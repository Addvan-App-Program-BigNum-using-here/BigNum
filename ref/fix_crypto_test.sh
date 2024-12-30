OS_TYPE=$(uname)

case "$OS_TYPE" in
    "Linux")

        echo "리눅스에서 실행될 코드"
        # Linux 특정 명령어들
        ;;
    "Darwin")
        echo "macOS에서 실행될 코드"
        # macOS 특정 명령어들
        ;;
    "MINGW"*|"CYGWIN"*|"MSYS"*)
        echo "Windows에서 실행될 코드"
        # Windows 특정 명령어들
        ;;
    *)
        echo "알 수 없는 운영체제입니다"
        exit 1
        ;;
esac