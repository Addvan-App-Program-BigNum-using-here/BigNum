# 1. 프로젝트 소개

### BigNum을 이용하여 암호 알고리즘을 만들고 Application 단에서 활용해보자!
공개키 암호에서 주로 사용되는 RSA, ECC와 같은 암호 알고리즘에서 필요한 큰 수를 다루는 라이브러리인 BigNum을 이용하여 암호 알고리즘을 만들어보고, 이를 Application 단에서 활용해보는 프로젝트입니다.

## ⚙ 기술 스택

### 🖥 BigNum 구현
<img alt="Html" src ="https://img.shields.io/badge/C-A8B9CC.svg?&style=for-the-badge&logo=C&logoColor=white"/>
<img alt="Html" src ="https://img.shields.io/badge/python-3776AB.svg?&style=for-the-badge&logo=python&logoColor=black"/>

### 🖥 협업
<img alt="Html" src ="https://img.shields.io/badge/github-181717.svg?&style=for-the-badge&logo=github&logoColor=white"/> <img alt="Html" src ="https://img.shields.io/badge/Notion-000000.svg?&style=for-the-badge&logo=Notion&logoColor=white"/> <img alt="Html" src ="https://img.shields.io/badge/Slack-4A154B.svg?&style=for-the-badge&logo=Slack&logoColor=white"/>

## 서비스 구조
<img src="https://github.com/user-attachments/assets/04c2889c-797f-4016-a895-5e2713595aab" width="660px"></img>

# 2. 소개 영상
[![Video Label](http://img.youtube.com/vi/ga8xQF-FVyk/0.jpg)](https://youtu.be/ga8xQF-FVyk)

# 3. 👩‍👩‍👧‍👧 팀 소개

|이름|사진|역할|               주소               |
|:---:|:---:|:---:|:------------------------------:|
| 유성환(****2240) |<img src="https://github.com/user-attachments/assets/4559042b-bd94-4a36-846a-18116705efcb" width="110px"></img>|조장, BigNum 구현, 백엔드 개발|   https://github.com/ISCMYOO   |
| 김찬혁(****2223) |<img src="https://github.com/user-attachments/assets/8df88784-1346-40fc-821d-196e392d370d" width="110px"></img>|BigNum 구현, 앱 개발|   https://github.com/BoomWin   |
| 이진현(****2102) |<img src="https://github.com/user-attachments/assets/acf4db23-d0e3-4c8a-adaf-b7384c8e78ea" width="110px"></img>|BigNum 구현| https://github.com/jinhyeon721 |


# 4. 사용법
## python Install
- 본 BigNum 프로젝트의 연산 검증은 python를 활용하기에 사전에 설치되어야 한다.
### MAC OS

```
# Homebrew 설치 (아직 설치하지 않았다면)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Python 설치
brew install python
```

### Linux OS (Ubuntu/Debian)
```
# 시스템 업데이트
sudo apt update

# Python 설치
sudo apt install python3 python3-pip

# Python 버전 확인
python3 --version
```

### Windows OS
```
# 하단 링크에서 설치
https://www.python.org/downloads/
```

## Make Install
- 또한 make 기능을 활용하여 프로젝트를 관리하기 때문에 make 설치도 사전에 진행되어야 한다.
### MAC OS
```
# Homebrew로 make 설치
brew install make

# Command Line Tools 설치 (XCode)
xcode-select --install
```

### Linux OS (Ubuntu/Debian)
```
# make 설치
sudo apt update
sudo apt install make build-essential
```

### Windows OS
```
# 방법 1
# Chocolatey 설치
# Chocolatey로 make 설치
choco install make

# 방법 2
# MinGW 설치
# MinGW로 make 설치

# 방법 3
# WSL
# WSL 설치 (PowerShell 관리자 모드)
wsl --install
sudo apt update
sudo apt install make
```

## BigNum Build
```
# 사전 git clone
git clone https://github.com/Addvan-App-Program-BigNum-using-here/BigNum.git

# 테스트 환경 세팅 쉘 코드 실행 권한 부여
chmod +x ./test_init.sh

# 테스트 환경 세팅 쉘 코드 실행
./test_init.sh

# 라이브러리 생성
make all

# 테스트 실행
make retest
```

### 테스트 시 환경변수 설명
#### `/ref/test/main_test.c` 파일
- 해당 파일을 확인하면 아래 4가지 함수가 존재한다.
- cmp_xxx_test 함수는 고정 입력 값에 대한 각 함수들의 성능 평가를 진행한다.
- rand_xxx_teset 함수는 랜덤 입력 값에 대한 각 함수들의 성능 평가를 진행한다.
- 따라서 테스트 하고 싶은 환경에 맞추어 주석 처리를 수행하면 된다.
- operate 단어가 포함된 함수는 연산에 대한 테스트를 진행한다.
- crypto 단어가 포함된 함수는 암호화에 대한 테스트를 진행한다.
```
// 고정 입력 값에 대한 성능 평가
if(cmp_operate_test() != Test_SUCCESS)    return 0;
if(cmp_crypto_test() != Test_SUCCESS)    return 0;

// 랜덤 입력 값에 대한 성능 평가
if(rand_operate_test() != Test_SUCCESS)    return 0;
if(rand_crypto_test() != Test_SUCCESS)    return 0;
```

#### `/ref/test/main_test.h` 파일
- `test_word_size` : 테스트 word 길이 (0으로 설정 시 테스트 word 길이는 랜덤)
- `test_word_size_limit` : word 사이즈가 랜덤으로 할당 시 최대 크기
- `test_size` : 테스트 반복 횟수

#### `/ref/operate.h` 파일
- `mul_karachuba_ratio` : 카라츄바를 사용하는 최대 비율
- `squ_karachuba_ratio` : 카라츄바 제곱을 사용하는 최대 비율
