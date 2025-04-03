# 🏆 B팀의 SSD 프로젝트 체크리스트 👍

## 📚 목차<a name="toc"></a> 

* [팀 소개](#팀-소개)
  * [팀명](#팀명)
  * [팀원 소개](#팀원-소개)
  * [Ground Rule](#Ground-Rule)
* [Team Project 미션 1](#Team-Project-미션-1)
  * [SSD](#SSD-미션1)
  * [Test Shell](#Test-Shell-미션1)
  * [Test Script](#Test-Script-미션1)
* [Team Project 미션 2](#Team-Project-미션-2)
  * [SSD](#SSD-미션2)
  * [Test Shell](#Test-Shell-미션2)
  * [Test Script](#Test-Script-미션2)
---

## 팀 소개
 ### 팀명
 Best Reviewers ( 최고의 Reviewer를 지향하는 사람들의 모임)
___

 ### 팀원 소개 
|팀원|역할|KonxID|mail|
|-----|-----|-----|-----|
|고병운님|팀장|byeongun.ko|kbwcap@gmail.com|
|김유증님|개발자|yuz010.kim|rladbwmd123@gmail.com|
|김윤제님|개발자|yunje.kim|kjo7811@naver.com|
|박화영님|개발자|hjy.park|raina.bak@gmail.com|

___
 
 ### Ground Rule  
 1.  아침 인사 하기
 2. 하던 일은 다음날로 미루고 17:30 퇴근  
 3. Approver 3 인 이상 , all review 받기
 4. commit은 16:00 시 전까지 만 올리기
 5. 마지막 Reviewer가 머지하고 Branch 삭제 
 6. Commit Message Format 사용 : [TAG] 커밋 내용 
 7. PR 수정 반영시  "Done" commnet 달기 
 8. Google 코딩 스타일 사용하기 
 9. 개발 Feature PR 전 시 Unit Test 가 포함하기 
 10. 퇴근하기 전에 인사하기

[🔝 목차로 돌아가기](#toc)

---

## Team Project 미션 1
 ### 기본 조건
   - `ssd_nand.txt`: 저장 파일 (초기 자동 생성)
   - `ssd_out.txt`: read 명령 결과 저장
   - 입력 형식
      - 인덱스 범위: 10진수로 `0 ~ 99` (총 100개 저장 공간) 
      - `value`: 16진수 (`0x`로 시작, 10자리) : `unsigned int`로 4바이트 데이터 처리
 ### 유의사항
   - 모든 상황에서 런타임 에러 발생 ❌
 ### SSD 미션1
   - [ ] `read` 기능 구현
      - 인덱스 0~99 벗어나면 `"ERROR"` 출력
      - LBS에 해당하는 값이 없으면 `0x00000000` 반환
      - `ssd_output.txt`에 결과 저장 (append)      
   - [ ] `write` 기능 구현
      - 인덱스 0~99 벗어나면 `"ERROR"` 출력
      - 정상적으로 실행되면 `ssd_nand.txt`에 값 저장
 ### Test Shell 미션1
   - [ ] `write`: SSD 호출 후 저장
   - [ ] `read`: SSD 호출 후 `ssd_output.txt`에서 읽어 출력  
     - 예: `LBA 3 0x00000000`
   - [ ] `exit`: 종료
   - [ ] `help`: 팀명, 팀원, 명령어 설명
         <details>
         <summary>📸 실행 결과 보기 (클릭)</summary>
         ![image](https://github.com/user-attachments/assets/658b937d-303f-45e4-8af0-8b9b8b36777d)
         </details>
   - [ ] `fullwrite`: 0~99까지 100회 write
   - [ ] `fullread`: 0~99까지 100회 read
   - [ ] 없는 명령어 입력 시 `"INVALID COMMAND"` 출력
   
 ### Test Script 미션1
  - [ ] Test Script 1
  - [ ] Test Script 2
  - [ ] Test Script 3

[🔝 목차로 돌아가기](#toc)

## Team Project 미션 2
### SSD 미션2
   - [ ] `erase` 기능 구현
       * E [LBA] [SIZE]  : size 0 보다 커야 함.
       * SSD 데이터 지우면 값은 0x00000000 으로 기록해야 한다.
       * 범위 벗어나는 경우 ERROR 표시
         - 범위 벗어나는 경우 있는 경우까지 돌릴지 전체 ERROR 처리 할 것인지 알아서. 
  - [ ] Command Buffer : 불필요한 command 줄이기.
       * SSD에서 받은 command 들을 SSD의 내부 저장소에 저장 후 한번에 처리..
       * max : 5
       * write, erase 명령어만 buffer 에 넣어준다.
       * buffer 폴더 생성해서 1_empty.txt .. 5_empty.txt 형식의 파일로 저장.
          -> buffer 폴더는 프로그램 실행할때 무조건 생성.
          -> 파일 내부에는 기록되는 항목이 없음.
          -> command 를 파일명에 기록해서 관리. 파일명은 알아서..
       * Ignore Command
          -> erase 명령어 수행하기 이전에 같은 lba를 write 하거나 erase 하는 명령어는 buffer에서 제거한다. ( 중복이 되거나 실행할 필요가 없는 command 는 삭제.. )
       * Merge Erase
          -> erase 명령어 횟수 줄이기...
       * Fast Read
          -> command buffer에 있는 경우 ssd_nand.txt 읽어들이지 않고 buffer에 있는 값 반환해주는 기능.
       * 
  - [ ] Flush
       * command buffer 에 있는 항목들을 한번에 실행하고 buffer 폴더에 있는 텍스트 파일명들을 empty 로 변경해 준다.
  - [ ] Comnnand Buffer : 명령어 모아 두는 곳
### Test Shell 미션2
   - [ ] `erase` 기능 구현
         E [LBA] [SIZE]  : size 0 보다 커야 할지는 여기서 판단해도 되고 안해도 된다. .
                           max 는 체크하지 않음.
                           LBA 범위는 체크 해야 함. 
   - [ ] `erase range` 기능 구현  : erase range [Start LBA] [End LBA]
                                    LBA 범위는 체크 해야 함.
   - [ ] Logger :
         포맷 정하기 : [25.04.04 09:00] 클래스명.함수명() : 로그메세지 -> 클래스명.함수명() 은 30칸만 확보하도록 한다.
         용량 : 10KB 단위로 관리.
         파일명 : latest.log => (10KB 넘어가는 경우 ) until_250403_09h_30m_11s.log 로 변경해서 저장해 준다.
         파일 압축하기 : until_*.log 파일이 2개이상 되는 경우 오래된 파일의 확장자를 until_*.zip 으로 변경해 준다.
   - [ ] Flush
 ### Test Script 미션2
  - [ ] Runner : 여러개의 테스트 스크립트를 모아서 한번에 수행 시킬 수 있는 기능.
       * 실행 방식은 아래와 같이 두가지.. 
         - 첫번째 : shell shell_scripts.txt : shell_scripts.txt 파일에 아래 항목들어 적어주면 순차적으로 실행 시켜주는 기능.
         - 두번째 : shell cli.. 명령어 입력해서 실행되는 방식.
           < shell_script.txt 파일에 적히는 내용. >
           1_FullWriteAndReadCompare
           2_PartialLBAWrite
           3_WriteREadAging
           4_EraseAndWriteAging
       * FAIL 이 발생하는 경우 진행을 멈춘다.
       * 명령의 체크.
       * 로그는.. 실행되는 순서대로..
           1_FullWriteAndReadCompare ___ Run... Pass...
           2_PartialLBAWrite ___ Run... Pass...
           3_WriteREadAging ___ Run... FAIL...
  - [ ] 새로운 Test Script 에 대한 처리 😵‍💫
       * Shell 을 다시 빌드하지 않고 처리하는 방법 추가해야함. 
 <br>
[🔝 목차로 돌아가기](#toc)
