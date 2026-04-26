# File Processing Assignments

숭실대학교 파일처리 수업의 구현 과제 모음입니다. C언어와 Unix 파일 I/O를 사용하여 파일 시스템의 핵심 개념들을 단계적으로 구현합니다.

---

## 과제 요약

| 과제 | 제목 | 핵심 내용 |
|---|---|---|
| 1 | 파일 I/O 프로그램 구현 | Unix 저수준 시스템 콜로 read, copy, insert, delete, overwrite, merge 6가지 파일 조작 구현 |
| 2 | Flash Device Driver 활용 | Flash 메모리 에뮬레이터 위에서 페이지 단위 쓰기/읽기 및 블록 단위 erase 구현. 덮어쓰기 시 copy → erase → 재기록 처리 |
| 3 | Block Mapping FTL 구현 | LSN → LBN → PBN 주소 변환 테이블 관리. 덮어쓰기 시 free block 교체 방식으로 Flash 특성 극복 |
| 4 | 레코드 저장 및 검색 | 학생 레코드를 가변 길이/delimiter 방식으로 파일에 저장. 파일·페이지·레코드 3단계 계층 구조 구현, 삽입·검색 지원 |
| 5 | 레코드 삭제 및 고도화된 삽입 | 삭제 레코드를 Linked List + Stack으로 관리. 삽입 시 First-Fit으로 삭제 공간 재활용 |

---

## 핵심 개념: 페이지와 레코드 단위로 읽고 쓰는 이유

과제 2~5에 걸쳐 공통으로 적용되는 원칙은 **파일을 바이트 단위가 아닌 페이지(page)와 레코드(record) 단위로 읽고 쓴다**는 것입니다.

### 왜 페이지 단위인가?

디스크나 Flash 메모리 같은 저장장치는 물리적으로 일정한 크기의 블록/페이지 단위로만 데이터를 읽고 씁니다. 단 1바이트를 수정하더라도 내부적으로는 해당 페이지 전체를 메모리로 읽어온 뒤(read), 메모리에서 수정하고, 수정된 페이지 전체를 다시 저장장치에 쓰는(write) **Read-Modify-Write** 과정이 반드시 발생합니다. 이 특성을 무시하고 임의의 바이트 위치에 직접 접근하면 나머지 데이터가 손상되거나, Flash 메모리처럼 덮어쓰기가 불가능한 장치에서는 아예 동작하지 않습니다.

따라서 과제 4·5의 레코드 파일도 512B 페이지를 I/O의 최소 단위로 설계했습니다. 레코드 하나를 읽거나 추가할 때도 반드시 해당 페이지 전체를 먼저 메모리 버퍼(`pagebuf`)로 읽어온 뒤(`readPage`), 버퍼 위에서 수정하고, 버퍼 전체를 다시 파일에 저장(`writePage`)합니다.

### 왜 레코드 단위인가?

학생 정보처럼 논리적으로 하나의 묶음인 데이터는 레코드라는 단위로 다루어야 검색·삽입·삭제 연산을 일관되게 처리할 수 있습니다. 페이지 안에서 레코드의 위치는 페이지 헤더의 **offset 배열**로 추적합니다. 각 레코드의 마지막 바이트 위치를 offset으로 기록해두면, 임의의 레코드 번호가 주어졌을 때 앞뒤 offset의 차이로 해당 레코드의 위치와 길이를 바로 계산할 수 있습니다. 레코드 자체는 `#` 구분자를 사용하는 **가변 길이(variable-length)** 방식으로 저장해 공간 낭비를 줄입니다.

### 3단계 계층 구조 (과제 4·5)

```
[File]  readFileHeader / writeFileHeader  →  총 페이지 수 관리
  └─ [Page]  readPage / writePage         →  512B 단위 I/O
       └─ [Record]  getRecFromPagebuf / writeRecToPagebuf  →  offset 배열로 레코드 접근
```

레코드를 수정하는 모든 연산은 이 3단계를 순서대로 거칩니다. 덕분에 디스크 I/O 횟수를 페이지 단위로 최소화하면서도, 응용 계층에서는 레코드라는 논리적 단위로만 데이터를 다룰 수 있습니다.

### Flash 메모리에서의 페이지 단위 I/O (과제 2·3)

Flash 메모리는 **덮어쓰기 불가(out-of-place update)** 라는 제약이 있습니다. 이미 기록된 페이지를 수정하려면 반드시 블록 전체를 erase(0xFF로 초기화)한 뒤에야 쓸 수 있습니다. 이 제약을 소프트웨어적으로 극복하기 위해 과제 2에서는 임시 버퍼 블록을 이용한 copy → erase → 재기록 절차를, 과제 3에서는 free block 교체 방식의 Block Mapping FTL을 구현합니다. FTL은 운영체제·파일시스템과 Flash 물리 장치 사이에서 논리 주소(LSN)를 물리 주소(PPN)로 투명하게 변환해주는 역할을 합니다.

---

## Assignment 1 — 파일 I/O 프로그램 구현

> **디렉토리**: `Assignments_1/`

Unix 저수준 시스템 콜(`open`, `read`, `write`, `lseek`, `ftruncate`, `close`)을 사용하여 6가지 파일 조작 프로그램을 구현합니다.

| 프로그램 | 사용법 | 기능 |
|---|---|---|
| `read.c` | `a.out <offset> <bytes> <file>` | 파일의 offset 위치에서 bytes만큼 읽어 출력. bytes가 음수이면 offset 이전 방향으로 읽음 |
| `copy.c` | `a.out <src> <dst>` | 소스 파일을 대상 파일로 복사 |
| `insert.c` | `a.out <offset> <data> <file>` | 파일의 offset+1 위치에 데이터를 삽입하고, 기존 데이터를 뒤로 밀어냄 |
| `delete.c` | `a.out <offset> <bytes> <file>` | 파일의 offset 위치에서 bytes만큼 데이터를 삭제. bytes가 음수이면 offset 이전 방향으로 삭제 |
| `overwrite.c` | `a.out <offset> <data> <file>` | 파일의 offset 위치부터 데이터를 덮어씀 |
| `merge.c` | `a.out <file1> <file2> <file3>` | file1과 file2를 순서대로 연결하여 file3에 저장 |

---

## Assignment 2 — Flash Device Driver 활용

> **디렉토리**: `Assignments_2/`

Flash Device Driver(`flashdevicedriver.c`)가 제공하는 `dd_read()`, `dd_write()`, `dd_erase()` 인터페이스를 활용하여 Flash Memory 에뮬레이터와 페이지 단위 I/O를 구현합니다.

**Flash 메모리 구조** (`flash.h`)

- 페이지: Sector 512B + Spare 16B = 528B
- 블록: 4 페이지 = 2112B
- 빈 상태(empty)를 나타내기 위해 `0xFF`로 초기화

**구현 기능** (`ftl.c`)

```
a.out c <flash_file> <block_count>     # Flash 메모리 파일 초기화 (전체 0xFF로 erase)
a.out w <flash_file> <ppn> <sector> <spare>  # 페이지 쓰기
a.out r <flash_file> <ppn>             # 페이지 읽기 (sector + spare 출력)
a.out e <flash_file> <pbn>             # 블록 erase
```

**페이지 쓰기 핵심 로직**: 이미 데이터가 있는 페이지에 쓸 때는 Flash의 덮어쓰기 불가 특성 때문에 아래 절차를 수행합니다.

1. 해당 블록의 나머지 페이지들을 블록 0(임시 버퍼 블록)으로 복사
2. 기존 블록 erase
3. 블록 0의 데이터를 원래 블록으로 재복사
4. 블록 0 erase
5. 원하는 페이지에 새 데이터 write

연산 완료 후 총 `#pageread`, `#pagewrites`, `#blockerases` 횟수를 출력합니다.

---

## Assignment 3 — Flash Memory Block Mapping FTL 구현

> **디렉토리**: `Assignments_3/`

Block Mapping 방식의 FTL(Flash Translation Layer)을 구현합니다. 파일 시스템은 논리 섹터 번호(LSN)를 사용하며, FTL이 이를 물리 주소(PPN)로 변환합니다.

**Flash 메모리 구조** (`blockmapping.h`)

- `PAGES_PER_BLOCK = 4`, `BLOCKS_PER_DEVICE = 16`
- 데이터 블록 수: 15 (나머지 1개는 free block으로 예약)
- 총 논리 페이지 수: 60 (LSN: 0~59)

**구현 기능** (`ftl.c`)

| 함수 | 설명 |
|---|---|
| `ftl_open()` | Address Mapping Table(LBN→PBN) 초기화 (-1로 설정) |
| `ftl_read(lsn, buf)` | LSN을 LBN과 offset으로 분리 → Mapping Table로 PBN 조회 → `dd_read()` 호출 |
| `ftl_write(lsn, buf)` | 최초 쓰기: 다음 빈 블록 할당. 덮어쓰기: free block에 유효 페이지 복사 후 기존 블록 erase, Mapping Table 갱신 |
| `ftl_print()` | 전체 LBN→PBN 매핑 테이블과 현재 free block 번호 출력 |

spare 영역에 LSN을 기록하여 유효/무효 페이지를 구분하며, 덮어쓰기 시 free block 교체 방식으로 wear-leveling을 간접 지원합니다.

---

## Assignment 4 — 레코드 저장 및 검색

> **디렉토리**: `Assignments_4/`

학생 정보를 가변 길이 레코드 방식으로 바이너리 파일에 저장하고, 임의의 필드를 키로 검색하는 프로그램을 구현합니다.

**레코드 파일 구조**

```
[File Header 16B]
  - 전체 페이지 수 (2B) + reserved (14B)

[Page 512B] × N
  [Page Header 64B]
    - #records (2B) + free space (2B) + reserved (4B) + offset 배열 (2B × 레코드 수)
  [Data Area 448B]
    - 가변 길이 레코드 (delimiter '#' 방식으로 필드 구분)
```

**학생 레코드 필드** (`student.h`): `ID`, `NAME`, `DEPT`, `YEAR`, `ADDR`, `PHONE`, `EMAIL`

**사용법**

```bash
# 레코드 삽입 (-i)
a.out -i students.dat "ID=20201234" "NAME=gdhong" "DEPT=Computer Science" \
      "YEAR=3" "ADDR=Dongjak-Gu,Seoul" "PHONE=02-828-0567" "EMAIL=gdhong@ssu.ac.kr"

# 레코드 검색 (-s)
a.out -s students.dat "NAME=gdhong"
```

**주요 구현 함수**

| 함수 | 역할 |
|---|---|
| `pack()` / `unpack()` | STUDENT 구조체 ↔ `#` 구분 레코드 문자열 직렬화/역직렬화 |
| `readPage()` / `writePage()` | 페이지 번호 기반 바이너리 페이지 I/O |
| `getRecFromPagebuf()` / `writeRecToPagebuf()` | 페이지 버퍼 내 offset을 이용한 레코드 접근 |
| `insert()` | 마지막 페이지에 레코드 추가. 공간 부족 시 새 페이지 할당 |
| `search()` | 전체 페이지 순회하며 키 값 일치 레코드 검색 |
| `ID_duplication_test()` | 삽입 전 학번 중복 여부 검사 |

---

## Assignment 5 — 레코드 삭제 및 고도화된 삽입

> **디렉토리**: `Assignments_5/`

Assignment 4를 확장하여 레코드 삭제(delete) 기능과 삭제된 공간을 재활용하는 고도화된 삽입(insert)을 구현합니다.

**삭제 레코드 관리 (Linked List + Stack)**

삭제된 레코드는 실제로 제거되지 않고, 해당 자리에 delete mark와 이전 삭제 레코드의 주소를 기록합니다.

```
삭제된 레코드 내용: *<Page_num><Record_num>  (각 2B)
```

- 삭제 레코드 리스트는 **스택(LIFO)** 방식으로 관리
- 리스트의 head(최근 삭제 레코드 위치)는 **파일 헤더의 reserved 영역** 앞 4B에 저장
- 첫 삭제 시 (이전 삭제 레코드 없음): `Page_num = -1`, `Record_num = -1`
- 레코드 삭제 후에도 페이지 헤더의 `#records`와 `free space`는 변경하지 않음

**고도화된 삽입 (First-Fit)**

삽입 시 삭제 레코드 리스트를 순서대로 탐색하여 새 레코드가 들어갈 수 있는 **첫 번째 공간(first-fit)**을 사용합니다. 적합한 공간이 없으면 기존 방식대로 파일 끝에 append 합니다.

**사용법**

```bash
# 레코드 삭제 (-d)
a.out -d students.dat "ID=20091234"

# 레코드 삽입 (-i) — 삭제 공간 재활용 포함
a.out -i students.dat "ID=20201234" "NAME=gdhong" ...
```

**추가/수정된 함수**

| 함수 | 역할 |
|---|---|
| `delete()` | 학번으로 레코드 검색 후 delete mark 기록 및 삭제 리스트 스택에 push |
| `searchByID()` | 학번 키로 레코드 검색 후 page 번호와 record 번호도 반환 |
| `insert()` | first-fit으로 삭제 공간 재활용. 없으면 파일 끝에 추가 |

---

## 개발 환경

- **OS**: Linux Ubuntu 22.04 LTS
- **컴파일러**: gcc 13.2
