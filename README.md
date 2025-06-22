# Embedded in Automotive

###### `Sử dụng STM32F103C8`
---

<details>
  <summary><h3>Bài 1: Setup KeilC</h3></summary>

## I. Blink Led PC13 bằng thanh ghi
(PC13: Chân 13 của GPIO C13)

> Cấp xung clock cho ngoại vi
>
> Cấu hình chế độ chân ngoại vi
>
> Sử dụng ngoại vi

### 1. Cấp xung clock cho ngoại vi

- Cấp xung clock cho GPIO từ APB2 bằng thanh ghi RCC_APB2ENR.
  
  (RCC: Module đang làm việc; APB: Tên đường bus; 2ENR: Chức năng ngoại vi)

```c
   RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // Cap clock cho ngoai vi
```

### 2. Cấu hình chế độ chân ngoại vi

- Sử dụng 2 thanh ghi GPIOx_CRL và GPIOx_CRH để cấu hình chân PC13 làm ngõ ra Push-Pull, tốc độ 50MHz.

  (Một GPIO có 16 chân, chia 2 thanh ghi CRL: 0-7; CRH: 8-15)

- Nháy LED PC13-> Dùng thanh ghi CRH (**GPIOx_CRH**), CRH có 2 loại là CRFx và MODEx.

*Push-pull: 1 0

*Open drain: Chỉ kéo xuống 0, không làm gì thì floating(lơ lửng).

![](https://github.com/hthuan02/Embedded_Automotive/blob/main/Bai1_Setup-KeilC/img/cau_hinh_che_do_chan.png)

```c
    GPIOC->CRH &= ~(GPIO_CRH_CNF13| GPIO_CRH_MODE13); // Reset cac bit CNF13 va MODE13 trong thanh ghi GPIOC_CRH.
											
    GPIOC->CRH |= GPIO_CRH_MODE13_0; // MODE13[1:0] = 11: Output mode, max speed 50 MHz
    GPIOC->CRH |= GPIO_CRH_MODE13_1; 	
    GPIOC->CRH &= ~GPIO_CRH_CNF13_0; // CNF13[1:0] = 00: General purpose output push-pull
    GPIOC->CRH &= ~GPIO_CRH_CNF13_1;

```

### 3. Sử dụng ngoại vi

- Khởi tạo hàm Deley
```c
      void delay(__IO uint32_t timedelay){ 
	      for(int i=0; i < timedelay; i++){}
        }
```
 
- Điều khiển LED: Sử dụng thanh ghi GPIOx_ODR để điều khiển LED nhấp nháy qua mã.
 
(**GPIOx_ODR**: Là thanh ghi giúp ghi tín hiệu điện áp ra chân tương ứng, 16 Bit là 16 Chân)

```c
    while(1){
	GPIOC->ODR |= (1 << 13); // LED tắt (dịch số 1 qua bên trái 13 lần)
  	delay(10000000);
  	GPIOC->ODR &= ~(1 << 13); // LED sáng ()
  	delay(10000000);

```
- Để thay thế câu lệnh dịch Bit ở hàm trên, thì khai báo hàm ghi tín hiệu điện áp.

```c
    void WritePin(GPIO_TypeDef *GPIO_Port, uint8_t Pin, uint8_t state)
    {
	if(state == HIGH)
	GPIO_Port->ODR |= (1<<Pin);
	else
	GPIO_Port->ODR &= ~(1<<Pin);
    }
```
## II. Đọc trạng thái nút nhấn bằng thanh ghi
(PA0: Button)

### 1. Cấp xung clock cho ngoại vi

```c
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN; // Cap xung clock cap cho GPIOA và GPIOC
```
### 2. Cấu hình chế độ chân ngoại vi

- PC13: LED -> GPIOx_CRH

- PA0: Button -> CRL(CNF & MODE): Reset bit của CNF0 & MODE0

```c
    // Cấu hình cho Button PA0
    GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0); // Reset trang thai CNF & MODE

    // CNF0 = 10: Input with pull-up/pull-down		
    GPIOA->CRL |= GPIO_CRL_CNF0_1; // Đưa Bit thứ 1 của CNF0 lên 1 (|=)
    GPIOA->CRL &= ~GPIO_CRL_CNF0_0; // Bit thứ 0 của CNF cho xuống 0 (&= ~)	       
    GPIOA->ODR |= GPIO_ODR_ODR0; // Set chân ODR0, PA0 là input pull-up

    // Cấu hình cho Led PC13
    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13); 
    GPIOC->CRH |= GPIO_CRH_MODE13_1;
    GPIOC->CRH &= ~GPIO_CRH_CNF13; 
```
![](https://github.com/hthuan02/Embedded_Automotive/blob/main/Bai1_Setup-KeilC/img/Pull-up-down-res.png)

### 3. Sử dụng ngoại vi
(IDR: Thanh ghi ReadOnly tín hiệu điện áp)

```c
    while(1){
	if((GPIOA->IDR & (1 << 0)) == 0) // Đọc trạng thái nút nhấn
	{
		GPIOC->ODR = 0 << 13;   // Nếu PA0 = 0 -> PC13 = 0
	}
	else
	{
		GPIOC->ODR = 1 << 13;   // Nếu PA0 = 1 -> PC13 = 1
	}
    }

```

</details>


<details>
  <summary><h3>Bài 2: GPIO</h3></summary>

## I. Blink Led PC13 bằng thư viện SPL
(SPL: Standard Peripheral Firmware Library - Thư viện STM32F10x SPL)

> Khi tạo file phải chọn 2 thư viện: RCC, GPIO trong cài đặt Environment.
### 1. Cấp clock cho ngoại vi

- Có 3 đường bus chính: APB1, APB2, AHB -> Có 3 hàm cấp clock cho ngoại vi.

<img src="https://github.com/hthuan02/Embedded_Automotive/blob/main/Bai2_GPIO/img/Duongbus-APB2-APB1-AHB.png" alt=" " width="600"/>

  **RCC_[Tên đường bus]_[Tên hàm cấp clock trên đường bus tương ứng]**

```c
    RCC_APB1PeriphClockCmd
    RCC_APB2PeriphClockCmd
    RCC_AHBPeriphClockCmd

```

### 2. Cấu hình cho ngoại vi
- Tạo 1 struct `GPIO_InitTypeDef` cấu hình cho 3 tham số: Pin, Speed, Mode.

```c
    GPIO_InitTypeDef GPIO_InitStruct; // Kiểu dữ liệu - Tên struct
```
- Sau đó, tạo 1 hàm lưu giá trị cài đặt 3 tham số vào thanh ghi
- 
```c
    GPIO_Init(GPIOC, &GPIO_InitStruct); // Tên ngoại vi, địa chỉ của Struct
```

### 3. Sử dụng ngoại vi
```c
    while(1){
	GPIO_SetBits(GPIOC, GPIO_Pin_13); // Ghi 1 ra PC13
	delay(10000000);
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);// Ghi 0 ra PC13
	delay(10000000);
    }
```
</details>

<details>
  <summary><h3>Bài 3: Interrupt & Timer</h3></summary>

## I. Interrupt (Ngắt)
### 1. Ngắt là gì?

> Ngắt là 1 sự kiện khẩn cấp xảy ra trong hoặc ngoài MCU. Nó yêu cầu MCU dừng chương trình chính `main()` và thực thi **chuơng trình ngắt.**
>
> Các hàm ngắt có địa chỉ cố định trong bộ nhớ -> Vector Ngắt.

**Các loại ngắt thông dụng:** Reset, ngắt ngoài, Timer, nngắt truyền thông.

![](https://github.com/hthuan02/Embedded_Automotive/blob/main/Bai3_Interrupt_Timer/img/Ngat.png)

### 2. Ngắt ngoài

> Xảy ra khi có **thay đổi điện áp** trên các chân GPIO được **cấu hình làm ngõ vào ngắt.**

Có 4 loại ngắt ngoài:

- **LOW:** Kích hoạt ngắt khi chân ở mức thấp.
 
- **HIGH:** Kích hoạt ngắt khi chân ở mức cao.

- **Rising:** Ngắt khi trạng thái của chân chuyển từ thấp -> cao.

- **Falling:** Ngắt khi trạng thái của chân chuyển từ cao -> thấp.

### 3. Ngắt Timer

> Xảy ra khi giá trị trong thanh ghi đếm của timer tràn.
>
> Vì là ngắt nội trong MCU, phải reset giá trị thanh ghi timer để có thể tạo lần ngắt tiếp theo.

(Nếu không reset giá trị thanh ghi đếm thì ngắt timer sẽ đếm từ 0-255 rồi mới reset về 0, chứ không reset khi đến giá trị chỉ định)

###  4. Ngắt truyền thông

> Xảy ra khi có sự kiện **truyền/nhận** dữ liệu giữa MCU với các thiết bị bên ngoài hay MCU với nhau.
>
> Các phương thức giao tiếp như: SPI, I2C, UART,... được sử dụng để ngát truyền thông.

### 5. Độ ưu tiên ngắt

>  Độ ưu tiên ngắt sẽ khác nhau ở mỗi ngắt.
>
>  Khi có nhiều ngắt xảy ra đồng thời, ngắt nào có độ ưu tiên cao hơn thì sẽ thực thi trước.

Trong STM32 ngắt có số ưu tiên thấp thì độ ưu tiên càng cao.


**PC(Program Counter):** Là thanh ghi luôn trỏ đến ô nhớ chứa lệnh tiếp theo trong chương trình.

_VD: Quy trình chạy của Program Counter:_
![](https://github.com/hthuan02/Embedded_Automotive/blob/main/Bai3_Interrupt_Timer/img/DO_UU_TIEN.png)

```
   PC: 0x01 -> 0x02 -> 0x03 pc:0x04: Lưu vào Stack(Last In - First Out)
   // Ngắt truyền thông(UART)
   PC: 0xD4 -> 0xD5 -> 0xD6 pc:0xD7( Lưu vào Stack -> Đỉnh stack)

   // Nngắt timer, do mức độ ưu tiên cao hơn, nên sẽ thực hiện hết ngắt này.
   PC: 0xB2 -> 0xB3 ... 0xB9 

   /* Sau khi xong ngắt mức độ ưu tiên cao nhất,
   *  PC trỏ đến đỉnh của stack.
   *  PC trỏ đáy của stack.
   */
   PC: 0xD7 -> 0xD8 ... 0xE2
   PC: 0x04 -> 0x05 ... 0xA1
```

## II. Timer

### 1. Timer là gì?

**đếm lên:** Từ 0 đến giá trị mình mong muốn sẽ tràn -> reset lại.

**đếm xuống:** Từ giá trị mong muốn đếm về 0, vượt qua 0 tràn tới -1 -> reset.

> Timer là 1 mạch digital logic(đếm nhị phân), nó sẽ đếm sau mỗi chu kỳ clock nhất định (đếm lên hoặc xuống).
> 
> Timer có thể hoạt động ở chế độ nhận xung clock từ tín hiệu ngoài, bộ dao động hoặc CPU. Có thể cấp xung vào 1 chân cho MCU và dùng xung đó điều khiển timer (tạo 1 nút nhấn, bộ đếm sẽ tăng lên sau mỗi lần nhấn nút).
>
> Timer có chế độ khác như PWM, định thời,...

**STM32F103 có 7 timer**

### 2. Cấu hình Timer
#### 2.1 Cấp xung Clock

```c
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
```

#### 2.2 Cấu hình chế độ chân 

Tạo 1 struct `TIM_TimeBaseInitTypeDef` cấu hình cho 4 tham số: ClockDivision, Prescaler, Period, CounterMode.

```c
    TIM_TimeBaseInitTypeDef TIM_InitStruct; 
```
 
- **ClockDivison**: Chia bớt Clock, cấp vào Timer2 đếm chậm hơn.

     - Nếu không làm gì thì RCC có thể cấp được 72MHz xung Clock
     
     - DV1: Không chia; DV2: Chia 2; DV4: 

```c
    TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1; // 72MHz/1 = 72MHz; 1s tao duoc 72000000 dao dong; 1dao dong 1/72000000s 
```

- **Prescaler**: Cho phép sau bao nhiêu dao động, đếm lên 1 lần.

     - Kiểu dữ liệu uint16_t(max) = 65535
  
     - Vì 1s đếm lên 1 lần -> 1ms (1/1000s) đếm lên 1 lần <=> Cấu hình: 72000 > 65535
  
     - Cách 1: Lấy 0.1ms đếm lên 1  lần <=> Cấu hình: 7200

     - Cách 2: `DIV2` còn 36MHz -> 1ms đếm lên 1 lân <=> Cấu hình: 36000

```c
     TIM_InitStruct.TIM_Prescaler = 7200-1; // 1s dem len 1 lan; (-1) vi Timer dem tu 0->7199
```
- **Period**: Quy định sau bao nhiêu lần đếm, thì Timer đếm lại từ đầu.

      - Để Timer đếm đếm giá trị tối da(uint16_t) là 65536-1
      - Nếu muốn ngắt Timer sau 0, là 10-1
  
```c
     TIM_InitStruct.TIM_Period = 65536-1; 
```
- **CounterMode**: Chọn chế độ đếm: Đếm lên, đếm xuống, căn giữa(khó kiểm soát),...

```c
     TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up; // Dem tu 0 len
```

- Cuối cùng, cài đặt cấu hình cho 4 tham số trên
```c
     TIM_TimeBaseInit(TIM2, &TIM_InitStruct); 
     TIM_Cmd(TIM2, ENABLE); // Gọi cmd để ngoại vi bắt đầu hoạt động
```

#### 2.3 Sử dụng ngoại vi
(Tìm trong Function trên KeilC)

- Hai hàm để sử dụng Timer: GetCounter, SetCounter.

   - **GetCounter:** Xem giá trị hiện tại đếm đến đâu, và trả về giá trị thanh ghi đếm.
 
   - **SetCounter:** Thay đổi giá trị trong thanh ghi đến, để có giá trị mới đếm lên hoặc đếm xuống.

_VD: Hàm Delay ms_

- Mỗi lần gọi hàm `delay_ms` Timer đếm lại từ đầu `SetCounter(TIM2,0)`.

- `while` liên tục kiểm tra giá trị của Timer2, nếu thời gian `TIM2 < timedelay * 10` vẫn ở trong `while` -> Khi `TIM2` lớn hơn thì thoát ra.

- `TIM2` = 0,1ms đếm lên 1 lần -> `TIM2` đếm lên 10 lần đc 1. Vì vây, `timedelay * 10` ms

```c
     void delay_ms(uint8_t timedelay)
     {
	TIM_SetCounter(TIM2,0);
        while(TIM_GetCounter(TIM2) < timedelay * 10){}
     }

```
</details>

<details>
  <summary><h3>Bài 4: Communication Protocols</h3></summary>

## 1.Truyền nhận dữ liệu là gì?

> Là quá trình trao đổi tín hiệu điện áp của MCU A đến MCU B. Gồm các tín hiệu 0 và 1(0v và 3.3V hoặc 5V).

_VD: Đối với 1 chuỗi như `hello`_

- MCU A sẽ dịch tuần tự từ kí tự `h` sang mã nhị phân 1 0(theo tiêu chuẩn ASCII) đến kí tự `o` qua MCU B.

- Việc truyền tín hiệu, sẽ truyền từng bit, bằng cách chênh lệch tăng hoặc giảm điện áp.

- **Nhưng:** Đối với tín hiệu có 3 bit 0 (hoặc 3 bit 1) liên tiếp giống nhau như (h: 0b 0110 1000). Thì việc truyền tín hiệu sẽ không có sự chênh lệch tín hiệu điện áp nào. Sẽ xảy ra sai xót trong truyền nhận dữ liệu.

Vì vậy, để phân biệt các bit liền kề giống thì các chuẩn giao tiếp ra đời như: SPI, I2C, UART,...

## 2. Ba chế độ của truyền nhận dữ liệu:

- **Đơn công**(simplex): Dữ liệu truyền theo 1 chiều, từ thiết bị gửi đến thiết bị nhận mà không có phản hồi ngược lại (Một thiết bị chỉ có thể truyền hoặc nhận).

_VD:_ Máy phát và máy thu (kết nối micro đến loa. Chỉ có truyền âm thanh một chiều từ micro đến loa).

- **Bán song công**(half-duplex): Dữ liệu truyền theo 2 chiều, nhưng không đồng thời. Tại 1 thời điểm chỉ một bên có thể truyền.

_VD:_ Bộ đàm.

- **Song công**(full-duplex): Dữ liệu truyền theo 2 chiều đồng thời; cả 2 bên có thể truyền hoặc nhận cùng một thời điểm. ->> SPI

_VD:_ Cuộc gọi điện thoại.

## 3. SPI
(SPI - Serial Peripheral Interface)

> Là chuẩn gao tiếp nối tiếp, đồng bộ
>
> Hoạt động ở chế độ song công (Có thể truyền hoặc nhận cùng 1 thời điểm).
>
> Một Master có thể giao tiếp với nhiều Slave (Master: Điều khiển quá trình giao tiếp SPI; Slave: là thiết bị thực hiện theo các yêu cầu của Master )
>
> SPI cho phép truyền dữ liệu với tốc độ rất nhanh.
> 
> Sử dụng 4 dây giao tiếp: SCK, MISO, MOSI, CS.

->> Đồng bộ: Là sự thống nhất của 2 MCU thời điểm nào là đọc, khi nào là ghi tín hiệu.
![](https://github.com/hthuan02/Embedded_Automotive/blob/main/Bai4_Comunication%20Protocols/img/SPI.jpg)

- **SCK**(Serial Clock): Là dây tạo ra tín hiệu đồng bộ. Master tạo ra xung tín hiệu truyền đến Slave nhận.

-  **MISO**(Master Input Slave Ouput): Master nhận dữ liệu - Slave truyền dữ liệu. Tín hiệu tạo bởi Slave và truyền đến Master.

-  **MOSI**(Master Output Slave Input): Master truyền dữ liệu - Slave nhận. Tín hiệu tạo bởi Master và truyền đến Slave nhận.

-  **CS/SS**(Slave Select/CS-Chip Select): Là chân giúp Master chọn được Slave để giao tiếp. Trong SPI Master có thể giao tiếp với nhiều Slave, và mỗi Slave ứng với 1 chân CS, nên Master muốn giao tiếp với Slave nào thì sẽ kéo dây CS về mức 0.

<img src="https://github.com/hthuan02/Embedded_Automotive/blob/main/Bai4_Comunication%20Protocols/img/Sodo_SPI.png" alt=" " width="440"/>

## Khung truyền SPI?

- Mỗi chip Master hoặc Slave có một thanh ghi 8 bit dữ liệu. Một byte dữ liệu được truyền theo cả hai hướng giữa Master và Slave.

- Bắt đầu quá trình, thì 1 Master chọn 1 Slave để giao tiếp, mà để chọn đc Slave giao tiếp thì Master kéo chân SS/CS của Slave tương ứng xuống mức 0.

- Sau khi chọn được Slave, Master sẽ cấp cho Slave 1 xung Clock. Cứ 1 xung clock truyền đi thì sẽ gửi tín hiệu đi qua chân (MOSI) và nhận tín hiệu này vào bằng chân (MISO).
->> Tại 1 thời điểm, vừa nhận vừa truyên nên nó là giao tiếp song công.

- Với mỗi Clock truyền đi, thì sẽ có 1 bit được truyền từ Master -> Slave.

- Sau khi truyền 1 bit kèm 1 clock, các bên nhận sẽ cập nhật lại thanh ghi và dịch 1 bit, và lặp lại quá trình đến khi đủ 8 bit trong thanh ghi. Cuối cùng, khi thanh ghi đủ 8 bit rồi thì đưa chân SS/CS lên lại 1, để dừng quá trình giao tiếp.

## Bốn chế độ hoạt động của SPI?

**CPOL**: Xác định mức điện áp của tín hiệu SCK.

- CPOL = 0: Ở trạng thái rãnh SCK = 0, khi truyền dữ liệu SCK lên 1.

- CPOL = 1: Ở trạng thái rãnh SCK = 1, khi truyền dữ liệu SCK xuống 0.

**CPHA**: Giúp chúng ta biết được thời điểm nào đọc dữ liệu, thời điểm nào chuyển đổi dữ liệu.

- CPHA = 0: Dữ liệu sẽ đến trước khi tín hiệu xung Clock đọc được.

- CPHA = 1: Clock được hình thành trước khi dữ liệu đến.

▶️▶️▶️Dựa vào CPOL và CPHA, chúng ta có 4 chế độ SPI

<img src="https://github.com/hthuan02/Embedded_Automotive/blob/main/Bai4_Comunication%20Protocols/img/CPOL_CPHA.png" alt=" " width="660"/>

## 4. I2C
(Inter-Integrated Circuit)

> Là chuẩn giao tiếp nối tiếp, đồng bộ.
>
> Hoạt động ở chế độ bán song công (Dữ liệu truyền theo 2 chiều, nhưng không đồng thời. Tại 1 thời điểm chỉ một bên có thể truyền).
>
> Một Master có thể giao tiếp với nhiều Slave.
> 
> Có 2 dây kết nối: SCL, SDA.

<img src="https://github.com/hthuan02/Embedded_Automotive/blob/main/Bai4_Comunication%20Protocols/img/unnamed.png" alt=" " width="600"/>

- **SCL**(Serial Clock): Là dây tạo tín hiệu đồng bộ, truyền-nhận dữ liệu từ Master đến Slave.

- **SDA**(Serial Data): Chân chứa dữ liệu được truyền đi.

## Chế độ hoạt động I2C?

> Khác với SPI chế độ hoạt động là (0) (1), còn ở I2C chế độ hoạt động là Open Drain. 

**Open Drain**: Khi I2C muốn điều khiển đường bus, sẽ hạ xuống là 0. Khi không điều khiển, thì sẽ thả trôi đường dây ở mức điện áp floating(sẽ không hiểu là mức 0 hay 1). Nên cần điện trở kéo lên nguồn thì bus mới lên 1.

## Khung truyền - nhận dữ liệu của I2C?

Giống với SPI, 1 bit truyền đi thì kèm theo 1 Clock.

- Đầu tiên, ở trạng thái rảnh SDA và SCL đều ở mức 1. Master bắt đầu quá trình truyền bằng cách kéo SDA xuống mức 0 trước SCL (điều kiện bắt đầu- Start Condition).

- Truyền địa chỉ: Truyền theo 1 frame(khung), Master gửi 7 bit địa chỉ để chọn Slave mà nó muốn giao tiếp, kèm theo 1 bit R/W (để xác định việc ghi hoặc đọc dữ liệu).

- Kiểm tra địa chỉ và bit R/W:

  - Slave nhận địa chỉ từ Master và so sánh với địa chỉ của mình. Nếu trùng khớp, Slave sẽ kiểm tra bit R/W.

  - R/W = 0: Master muốn ghi(gửi) dữ liệu vào Slave(Write).

  - R/W = 1: Master muốn đọc(nhận) dữ liệu từ Slave(Read).

- Xác nhận (Bit ACK): Sau khi nhận được 8 bit (7 bit địa chỉ và 1 bit R/W), Slave gửi lại một bit ACK (bit xác nhận) bằng cách kéo SDA xuống mức 0.

- Kiểm tra (Bit ACK): Master chờ nhận bit ACK từ Slave. Nếu SDA = 0 (gửi ACK thành công), Master tiếp tục truyền dữ liệu. Nếu SDA = 1 (gửi ACK thất bại), Master sẽ sử dụng timer để kiểm tra, Nếu sau một khoảng thời gian mà SDA vẫn chưa xuống 0, Master sẽ gửi lại dữ liệu. Nó sẽ tiếp tục cho đến khi đạt giới hạn truyền, nếu không nhận được bit ACK, Master sẽ dừng quá trình I2C.
  
- Sau khi nhận được ACK, Master tiếp tục gửi hoặc nhận 8 bit dữ liệu tiếp theo, với mỗi bit kèm theo 1 xung clock.

- Kết thúc: Khi dữ liệu đã truyền hết, Master gửi tín hiệu kết thúc bằng cách đưa SDA và SCL lên mức 1 để dừng quá trình giao tiếp I2C.
  
## 5. UART
(Universal Asynchronous Receiver-Transmitter)

> Chuẩn giao tiếp nối tiếp, không đồng bộ.
>
> Hoạt động ở chế độ song công (Có thể truyền hoặc nhận cùng 1 thời điểm).
>
> Chỉ 2 thiết bị giao tiếp với nhau, 1 Master chỉ giao tiếp 1 Slave
>
> Sử dụng 2 dây giao tiêp: TX, RX.

<img src="https://github.com/hthuan02/Embedded_Automotive/blob/main/Bai4_Comunication%20Protocols/img/img_temp_6449845cb3ff48-87888352-85375715.png" alt=" " width="500"/>

- **Tx**(Transmit): Chân truyền dữ liệu.

- **Rx**(Receive): Chân nhận dữ liệu. 

UART không có sự đồng bộ về gửi và nhận dữ liệu giữa 2 thiết bị, giải pháp là tạo 1 timer. Vì mỗi MCU có tần số xung nhịp khác nhau, dẫn đến thời gian delay của timer 2 MCU sẽ khác. 

▶️▶️▶️ Baundrate là tham số để thống nhất lại, đồng bộ thời gian truyền-nhận dữ liệu và thời gian delay của 2 MCU (ố Bit truyền/s).

## Khung truyền - nhận dữ liệu của UART?

- Mỗi lần truyền 1 là frame(khung), 1 frame bao gồm:

<img src="https://github.com/hthuan02/Embedded_Automotive/blob/main/Bai4_Comunication%20Protocols/img/1%20frame_uart.png" alt=" " width="500"/>

- Start bit: Không truyền dữ liệu thì Tx, Rx = 1. Đầu tiên MCU A hạ chân Tx từ 1 xuống 0 để truyền dữ liệu, sau đó delay 1 khoảng thời gian trong 1 Bit.

- Khi MCU A hạ Tx = 0, thì Rx trong MCU B sẽ đọc dữ liệu từ Tx(Rx =1), sau khoảng thời gian delay. MCU B sẽ hạ RX = 0 để tiên hành ghi dữ liệu.

- Data frame: gồm ban đầu 5-9 Bit. Thông thường là 8 Bit.

- Để gửi data frame:
   - MCU A(bên gửi) sẽ gửi 1 Bit và dịch Bit, tuần tự đến khi nào hết 8 Bit dữ liệu.
     
   - MCU B(bên nhận), sẽ đọc dữ liệu rồi dịch, tuần tự hết 8 bit.

  ▶️ Gửi và nhận cùng 1 thời điểm

- Sau khi gửi dữ liệu xong thì có 1 Bit Optional(Bit tùy chọn): Có Parity Bit, không có Parity Bit -> Giúp kiểm tra lỗi trong data frame: Quy luật chẵn và lẻ

   - Quy luật chẵn: (Số lượng Bit 1 của Data frame) + (Parity Bit) là số chẵn.
 
   - Quy luật lẻ: (Số lượng Bit 1 của Data frame) + (Parity Bit) là số lẻ.
  
  ▶️Nhược điểm: Không phát hiện được số Bit chẵn bị sai.
  
- Cuối cùng, stop bit: Có thể chiếm trong 1 Bit hoặc 2 Bit
   
   - Nếu 1 Bit: Đưa TX lên 1, delay thời gian trong 1 Bit để kết thúc quá trình.
 
   - Nếu 2 Bit: Đưa TX lên 1, và thời gian delayy gấp 2 lần thời gian braudrate ban đầu.
 
#### So sánh SPI & I2C? 

| Tiêu chí               | SPI (Serial Peripheral Interface)                          | I2C (Inter-Integrated Circuit)                    |
|------------------------|----------------------------------------------------------|--------------------------------------------------|
| **Chế độ truyền**      | Song công (full-duplex)                                   | Bán song công (half-duplex)                      |
| **Số dây kết nối**     | 4 dây: SCK, MISO, MOSI, CS                               | 2 dây: SCL, SDA                                   |
| **Tốc độ truyền**      | Tốc độ cao (lên đến vài MHz)                             | Tốc độ thấp hơn (thường 100 kHz, 400 kHz, 1 MHz) |
| **Địa chỉ thiết bị**    | Không có địa chỉ, chỉ dụng chân CS để chọn Slave        | Sử dụng địa chỉ 7 bit (hoặc 10 bit) cho các Slave |
| **Xác nhận**           | Không có cơ chế xác nhận tự động                         | Có cơ chế xác nhận (ACK/NACK)                     |

#### So sánh UART & I2C ?

| Tiêu chí               | UART (Universal Asynchronous Receiver-Transmitter)  | I2C (Inter-Integrated Circuit)                    |
|------------------------|-----------------------------------------------------|--------------------------------------------------|
| **Chế độ truyền**      | Song công (full-duplex)                             | Bán song công (half-duplex)                      |
| **Số thiết bị kết nối** | Chỉ 2 thiết bị (1 Master - 1 Slave)                | Nhiều thiết bị (1 Master - nhiều Slave)         |
| **Số dây kết nối**     | 2 dây: TX, RX                                       | 2 dây: SCL, SDA                                   |
| **Tốc độ truyền**      | Tốc độ cao, tùy thuộc vào baudrate (lên đến vài Mbps) | Tốc độ thấp hơn (thường 100 kHz, 400 kHz, 1 MHz) |
| **Đồng bộ**            | Không đồng bộ (sử dụng timer để đồng bộ)          | Đồng bộ (sử dụng SCL để đồng bộ dữ liệu)        |
| **Xác nhận dữ liệu**   | Không có cơ chế xác nhận tự động                   | Có cơ chế xác nhận (ACK/NACK)                     |
| **Cấu trúc dữ liệu**   | Gửi dữ liệu theo frame có Start, Data, Optional (Parity), Stop Bit | Gửi dữ liệu theo frame có địa chỉ và bit R/W   |

#### So Sánh SPI & UART?

| Tiêu chí               | SPI (Serial Peripheral Interface)                     | UART (Universal Asynchronous Receiver-Transmitter)  |
|------------------------|------------------------------------------------------|-----------------------------------------------------|
| **Chế độ truyền**      | Song công (full-duplex)                              | Song công (full-duplex)                             |
| **Số thiết bị kết nối** | Nhiều thiết bị (1 Master - nhiều Slave)             | Chỉ 2 thiết bị (1 Master - 1 Slave)                 |
| **Số dây kết nối**     | 4 dây: SCK, MISO, MOSI, CS                           | 2 dây: TX, RX                                       |
| **Tốc độ truyền**      | Tốc độ cao (lên đến vài MHz)                         | Tốc độ cao, tùy thuộc vào baudrate (lên đến vài Mbps) |
| **Đồng bộ**            | Đồng bộ (sử dụng SCK để đồng bộ dữ liệu)           | Không đồng bộ (sử dụng timer để đồng bộ)           |
| **Xác nhận dữ liệu**   | Không có cơ chế xác nhận tự động (ACK/NACK như I2C)                     | Không có cơ chế xác nhận tự động                    |
| **Cấu trúc dữ liệu**   | Gửi dữ liệu theo frame, không có địa chỉ thiết bị   | Gửi dữ liệu theo frame có Start, Data, Optional (Parity), Stop Bit |

</details>

<details>
  <summary><h3>Bài 5: SPI Software - SPI Hardware</h3></summary>

## I. SPI Software

### 1. Xác định chân

```c
	#define SPI_SCK_Pin GPIO_Pin_0
	#define SPI_MISO_Pin GPIO_Pin_1
	#define SPI_MOSI_Pin GPIO_Pin_2
	#define SPI_CS_Pin GPIO_Pin_3
	#define SPI_GPIO GPIOA
	#define SPI_RCC RCC_APB2Periph_GPIOA
```

### 2. Cấp xung RCC_Config, cấu hình GPIO_Config 

```c
void_RCC(){
	RCC_APB2PeriphClockCmd(SPI_RCC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}
```

- Cấu hình GPIO Master gồm: SCK, MISO, CS (Ouput); MOSI (Input). Nếu cấu hình Slave thì ngược lại Mode

```c
void GPIO_Config(){
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_SCK_Pin | SPI_MOSI_Pin | SPI_CS_Pin; // SCK, MISO, CS (ouput)
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_MISO_Pin;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; // Chan MISO(Input) -> Cau hinh chi co chuc nang doc, floating tranh sai xot
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO, &GPIO_InitStruct);
}
```
### 3. Hàm cấp clock SPI_Clock

- Chỉ cấp Clock cho chân SCK của Master

```c
void SPI_Clock(){
	// Ham viet tin hieu dien ap ra(WriteBit)
	GPIO_WriteBit(SPI_GPIO, SPI_SCK_Pin, Bit_SET);
	delay_ms(4);
	GPIO_WriteBit(SPI_GPIO, SPI_SCK_Pin, Bit_RESET);
	delay_ms(4);
}
```

### 4. Hàm setup trạng thái ban đầu master và slave

```c
// Master
void SPI_Setup(){
	GPIOA_WriteBit(SPI_GPIO, SPI_SCK_Pin, Bit_RESET);
	GPIOA_WriteBit(SPI_GPIO, SPI_CS_Pin, Bit_SET);
	GPIOA_WriteBit(SPI_GPIO, SPI_MOSI_Pin, Bit_RESET);
}
```

```c
// Slave
void SPI_Setup(){
	GPIOA_WriteBit(SPI_GPIO, SPI_MISO_Pin,Bit_RESET);
}
```

  ![](https://github.com/hthuan02/Embedded_Automotive/blob/main/Bai5_SPI%20Software%20-%20Hardware/img/TinHieu_CSK-MISO-MOSI-CS.png)

### 5. Hàm truyền và hàm nhận

#### Hàm truyền lần lượt 8 bit dữ liệu
  
- Kéo CS xuống 0.
   
  	- Truyền 1 Bit.
 
	- Dịch 1 Bit.

 	- Gửi Clock();

- Kéo CS lên 1.

```c
void SPI_Master_Transmit(uint8_t u8Data){ //0b10010000
	//0b10000000
	uint8_t u8Mask = 0x80; // Tao 1 bitmask de truyen, lay MSP(trong so ben trai-Lon nhat), LSP + all bit con lai = 0
 	uint8_t tempData; // Bit chua gia tri cac bit truyen di
	GPIO_WriteBit(SPI_GPIO, SPI_CS_Pin, Bit_RESET);
	delay_ms(1); // Delay dam bao co thoi gian de CS keo xuong muc 0
	
	for(int i =0; i< 8; i++){
		// (Data) AND (bitmask)
		tempData = u8Mask & u8Data;
			
			// Neu ca 2 gia tri la 1, thi MISO se ghi gia tri, SET 1
			// Neu ca 2 gia tri 1 0 (KTM toan tu AND), MISO van giu nguyen RESET_0
			if(tempData){
				GPIO_WriteBit(SPI_GPIO, SPI_MOSI_Pin, Bit_SET);
				delay_ms(1);
			}
			else{
                GPIO_WriteBit(SPI_GPIO, SPI_MOSI_Pin, Bit_RESET);
				delay_ms(1);
			}
			
			u8Data <<= 1;  // Dich qua ben Trai 1 bit
			Clock(); // 1 bit truyen + Cap 1 clock
	}
	// Het 8 Bit du lieu, keo CS len 1
	GPIO_WriteBit(SPI_GPIO, SPI_CS_Pin, Bit_SET);
	delay_ms(1);
}  	
  ```

#### Hàm nhận nhận lần lượt 8 bit dữ liệu.
	
- Kiểm tra CS ==0?.
	- Kiểm tra Clock==1?
	- Đọc data trên MOSI, ghi vào biến.
	- Dịch 1 bit.
- Kiểm tra CS==1?

```c
uint8_t SPI_Slave_Receive(void){
	uint8_t dataReceive = 0x00; //0b0000 0000
	uint8_t temp = 0x00; // Bien chua cac bit nhan vao
	
	while(GPIO_ReadInputDataBit(SPI_GPIO, SPI_CS_Pin)); 
	while(!GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin));
	// Doc lai chan SCK
	for(int i = 0; i< 8; i++){
		if(GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin)){ // Kiem tra, Neu SCK = 1
	
			while (GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin)){
				temp = GPIO_ReadInputDataBit(SPI_GPIO, SPI_MOSI_Pin); // Doc lien tuc MOSI(nhan) tu MISO(truyen)
			}
			// Dich bit
			dataReceive <<= 1; // Dich trai
			dataReceive |=temp;
		}
		// Kiem tra CS =1
		while(!GPIO_ReadInputDataBit(SPI_GPIO, SPI_SCK_Pin));
	}
	return dataReceive;
}
```

## II. SPI Hardware

### 1. Xác định chân

- Giao tiếp SPI Hardware trên STM32 chỉ có 2 bộ SPI:

  	- **PA (SPI1):** PA4 -> PA7
  	  
  	  ![](https://github.com/hthuan02/Embedded_Automotive/blob/main/Bai5_SPI%20Software%20-%20Hardware/img/SPI1.png)

	- **PA (SPI2):** PB12 -> PA15

	  ![](https://github.com/hthuan02/Embedded_Automotive/blob/main/Bai5_SPI%20Software%20-%20Hardware/img/SPI2.png)

### 2. Cấp clock RCC_Config, cấu hình GPIO_Config


### 3. Cấu hình SPI_Config

Cấu hình SPI_Config() trong 1 Struct là `SPI_InitTypeDef`, trong Struct có các tham số như:
	 
- **SPI_Mode**: Quy định chế độ hoạt động của thiết bị SPI. 

- **SPI_Direction**: Quy định kiểu truyền của thiết bị.

- **SPI_BaudRatePrescaler**: Hệ số chia clock cấp cho Module SPI.
   
- **SPI_CPOL**: Cấu hình cực tính của SCK . Có 2 chế độ:

	- SPI_CPOL_Low: Cực tính mức 0 khi SCK không truyền xung.

	- SPI_CPOL_High: Cực tính mức 1 khi SCK không truyền xung.

- **SPI_CPHA**: Cấu hình chế độ hoạt động của SCK. Có 2 chế độ:

	- SPI_CPHA_1Edge: Tín hiệu truyền đi ở cạnh xung đầu tiên.

	- SPI_CPHA_2Edge: Tín hiệu truyền đi ở cạnh xung thứ hai.
 
- **SPI_DataSize**: Cấu hình số bit truyền. 8 hoặc 16 bit.

- **SPI_FirstBit**: Cấu hình chiều truyền của các bit là MSB hay LSB.

- **SPI_CRCPolynomial**: Cấu hình số bit CheckSum cho SPI.

- **SPI_NSS**: Cấu hình chân SS là điều khiển bằng thiết bị hay phần mềm.

#### Lưu ý: 4 tham số CPOL, CPHA, DataSize, FirstBit phải giống nhau ở Master và Slave để truyền chính xác dữ liệu.



### 4. Các hàm gửi, nhận và lấy giá trị cờ

- **SPI_I2S_SendData**: Hàm gửi mặc định  là 16bit, nếu ghi 8bit thì sẽ kèm 8bit 0 ở đầu.
 
- **SPI_I2S_ReceiveData**: Hàm nhận data đọc được, gia trị trả về `uint16_t`, có thể ép kiểu `uint8_t`
 
- **SPI_I2S_GetFlagStatus**: Lấy giá trị cờ trong thanh ghi SPI.

	- SPI_I2S_FLAG_TXE (Transmit Buffer Empty = 0): Cờ báo truyền, cờ này sẽ set lên 1 khi bộ truyền trống (không có tín hiệu truyền).

	- SPI_I2S_FLAG_RXNE (Not Empty = 1): Cờ báo nhận, cờ này set lên 1 bộ nhận không trống.
		
  	- SPI_I2S_FLAG_BSY: Cờ báo bận set lên 1, khi không bận = 0

</details>

<details>
  <summary><h3>Bài 6: I2C Software - I2C Hardware</h3></summary>

</details>

<details>
  <summary><h3>Bài 7: UART Software - UART Hardware</h3></summary>

</details>

<details>
  <summary><h3>Bài 8: Interrupt (Continuous)</h3></summary>

</details>


<details>
  <summary><h3>Bài 9: ADC</h3></summary>

## 1. Định nghĩa

> **ADC**(Analog to Digital Converter) là một mạch điện tử giúp chuyển đổi tín hiệu tương tự (analog) sang tín hiệu số (digital) mà MCU có thể hiểu và xử lý được.

![](https://github.com/hthuan02/Embedded_Automotive/blob/main/Bai9_ADC/img/ADC.png)

- ADC hoạt động bằng cách chia mức tín hiệu tương tự thành nhiều mức khác nhau. Các mức được biểu diễn bằng các bit nhị phân 0 1.

- Để chia tín hiệu ra thành nhiều mức khác nhau thì dựa vào thông số, thông số đó là **độ phân giải**.

#### Độ phân giải (Resolution)
- Là số bit cần thiết để chứa hết các mức giá trị tín hiệu số (digital) sau quá trình chuyển đổi ở ngõ ra.

- Độ phân giải  càng cao thì càng nhiều mức tín hiệu -> Kết quả càng chính xác.

#### Tần số lấy mẫu 

- Là tốc độ lấy mẫu và số hóa của bộ chuyển đổi, thời gian giữa 2 lần số hóa càng ngắn độ chính xác càng cao. Khả năng tái tạo lại tín hiệu càng chính xác.

- Tần số lấy mẫu càng cao thì lấy được nhiều mẫu hơn -> Kết quả càng chính xác.

- **Chu kỳ lấy mẫu** là khoảng thời gian giữa các lần lấy mẫu, được tính bằng: thời gian lấy mẫu tín hiệu + thời gian chuyển đổi.

## 2. Sử dụng ADC trong STM32

- Trong STM32F103C8 có 2 bộ ADC, kết quả chuyển đổi được lưu trữ trong thanh ghi 16 bit.

- Bộ chuyển đổi ADC có độ phân giải mặc định là 12 bit. (kết quả sau khi bộ ADC chuyển đổi cũng 12 bit). Tức là, có thể chuyển đổi ra 2^{12} = 4096 giá trị ở ngõ ra tín hiệu số.

## 3. Chế độ hoạt động của bộ ADC
(Injected và Regular Channels).

Để 2 bộ hoạt động thì dùng nguồn kích hoạt phần mềm hoặc phần cứng. Hoặc nguồn kích hoạt ngoài **External Trigger** -> Chuyển đổi dữ liệu Analog - Digital.	

#### Regular Conversion
(Có 4 kiểu hoạt động)

- **Single**: ADC chỉ đọc 1 kênh duy nhất, khi nào yêu câu thì mới đọc (VD: Khi nhấn nút nó sẽ đọc; Còn khi thả ra hoặc đang giữ nút nó không đọc được).

- **Single Continous:** ADC đọc 1 kênh duy nhất, nhưng dữ liệu được đọc liên tục đến khi bắt buộc nó dừng lại (VD: Khi nhấn nút nó sẽ đọc dữ liệu liên tục, chỉ có thể dừng lại bằng kiểu khác chứ không dừng bằng nút nhấn được).

- **Scan: Multi - Channels:** Quét qua và đọc dữ liệu nhiều kênh, chỉ đọc khi được yêu cầu. (Nó sẽ chuyển đổi lưu vào thanh ghi DR tuần mỗi lần 1 kênh đến khi hết kênh).

▶️ Nhược điểm: Giá trị chuyển đổi sau bị đè lên giá trị trước trong thanh ghi DR.

▶️ Khắc  phục: Sau mỗi lần chuyển đổi, thì phải lấy giá trị được chuyển đổi trong thanh ghi DR ra -> Lưu vào 1 mảng. Giá trị tiếp theo thì lưu vào phần tử tiếp theo của mảng. 

- **Scan: Continous Multi - Channels Repeat:** Quét qua và đọc dữ liệu nhiều kênh, nhưng đọc liên tiếp nhiều lần như Single Continous

#### Injected Conversion:

Cũng là chuyển đổi từ Analog - Digital, nhưng có mức độ ưu tiên cao hơn Regular. Vì vậy, chỉ sử dụng khi chuyển đổi tín hiệu khẩn cấp.

## 4. Cấu hình ADC

- Cấp xung clock: ADC1 và ADC2 nằm trong đường bus APB2, nên cấu hình chung với GPIOA.

```c
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
     RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
```

- Cấu hình chân GPIO
     - Ta có, `A0 - A7 <=> IN0 - IN7`, `B0 <=> IN8`, `B1 <=> IN9`, `C0-C5 <=> IN10-IN15`.

       ![](https://github.com/hthuan02/Embedded_Automotive/blob/main/Bai9_ADC/img/PA_IN_trongADC.png)


- Cấu hình ADC:

     - **ADC_Mode**:  Cấu hình chế độ hoạt động cho ADC là Independent(đơn/ độc lập), Dual(Đồng bộ). Ngoài ra, còn có các chế độ ADC chuyển đổi tuần tự các kênh (regularly) hay chuyển đổi khi có kích hoạt (injected).
     
     - **ADC_NbrOfChannel**: Thành viên thứ 2, số kênh ADC dùng để chuyển đổi tín hiệu.

     - **ADC_ScanConvMode**: Cấu hình chế độ quét ADC lần lượt từng kênh. Enable nếu sử dụng chế độ quét này, 1 kênh thì DISABLE.
 
     - **ADC_ExternalTrigConv**: Enable để sử dụng tín hiệu trigger, chọn nguồn kích hoạt bên ngoài cho việc chuyển đổi ADC. Nếu không sử dụng thì `= ADC_ExternalTrigConv_None`

     - **ADC_ContinuousConvMode**: Cấu hình bộ ADC có chuyển đổi liên tục hay không, Enable để cấu hình ADC chuyển đổi lien tục, nếu cấu hình Disable, ta phải gọi lại lệnh đọc ADC để bắt đầu quá trình chuyển đổi.
       
     - **ADC_DataAlign**: Cấu hình căn lề cho data. Vì bộ ADC xuất ra giá trị 12bit, được lưu vào biến 16 hoặc 32 bit nên phải căn lề các bit về trái hoặc phải (thường căn lề phải, 4 bit đầu bên trái là: 0 0 0 0)


Cuối cùng, cũng trong hàm cấu hình ADC. Phải chọn tần số lấy mẫu và kênh ADC muốn làm việc.

- _[Bộ ADC sử dụng, kênh muốn làm việc, mưc độ ưu tiên(1: cao nhất-16: thấp nhất), tần số lấy mẫu(1cycles5= 1,5)]_
  
```c
     ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
```

- Hàm `cmd` cho phép ADC hoạt động.

```c
     ADC_Cmd(ADC1, ENABLE);
```

- Bắt đầu quá trình chuyển đồi.

```c
     ADC_SoftwareStartConvCmd(ADC1, ENABLE);
```
</details>

<details>
  <summary><h3>Bài 10: DMA & PWM</h3></summary>

## I. DMA
### 1. Định nghĩa

- **DMA**(Direct Memory Access) là một bộ giúp truyền dữ liệu, giao tiếp 2 ngoại vi với nhau hoặc giao tiếp ngoại vi với bộ nhớ một cách trực tiếp mà không cần đến CPU phải thực hiện từng bước truyền dữ liệu.

- CPU giao việc truyền-nhận dữ liệu cho DMA, còn CPU thực hiện việc khác. Giúp tăng hiệu suất chương trình hạn chế việc CPU bị chiếm dụng quá tải, hạn chế hiệu suất chương trình.

- DMA hoạt động sẽ không liên quan đến quá trình thực thi của CPU, nhờ đó DMA có để truyền nhận dữ liệu mọi lúc. Làm cho bộ nhớ của CPU trở nên nhẹ, thực hiện được nhiều thao tác một cách mượt mà.

### 2. Hoạt động của DMA

![](https://github.com/hthuan02/Embedded_Automotive/blob/main/Bai10_DMA/img/flow_DMA.png)

- **1.** Cấu hình và Kích hoạt DMA: CPU (1) thực hiện việc cấu hình và kích hoạt bộ điều khiển DMA (4) để nó có thể hoạt động.

- **2.** Yêu cầu từ Ngoại vi: Các thiết bị ngoại vi (5) sử dụng tín hiệu yêu cầu DMA (DMA Request) (6) để yêu cầu DMA (4) gửi hoặc nhận dữ liệu.

- **3.** Thực hiện Yêu cầu DMA: Khi nhận được yêu cầu từ tín hiệu DMA Request (6), DMA (4) bắt đầu thực hiện nhiệm vụ gửi hoặc nhận dữ liệu.

- **4.** Truyền Dữ liệu: DMA (4) lấy dữ liệu từ bộ nhớ SRAM (2) thông qua ma trận bus (Bus Matrix) (3). Dữ liệu được truyền qua các đường bus ngoại vi để truy cập các thanh ghi của thiết bị ngoại vi (5).

- **5.** Kết thúc và Thông báo: Sau khi quá trình truyền dữ liệu hoàn tất, DMA (4) kích hoạt ngắt để thông báo cho CPU (1) biết rằng quá trình đã hoàn thành.

### 3. DMA trong STM32

STM32F103C8T6 có 2 bộ DMA. DMA1 bao gồm 7 kênh, DMA2 bao gồm 5 kênh:

- Có 2 chế độ hoạt động.

- Mỗi kênh có thể được cấu hình riêng.

- Mỗi kênh có thể phục vụ cho nhiều ngoại vi khác nhau, nhưng không đồng thời.

- Có mức ưu tiên để lập trình cho các kênh

- Có thể sử dụng ngắt DMA với 5 cờ báo ngắt (DMA Half Transfer, DMA Transfer complete, DMA Transfer Error, DMA FIFO Error, Direct Mode Error).

#### 2 Chế độ hoạt động DMA

- **Normal mode:** DMA truyền dữ liệu cho đến khi truyền đủ 1 lượng dữ liệu giới hạn (có thể khai báo số lượng dữ liệu muốn truyền), khi truyền đủ số lượng khai báo DMA sẽ dừng hoạt động. Muốn nó tiếp tục hoạt động thì phải khởi động lại.

- **Circular:** DMA truyền dữ liệu cho đên khi đủ số lượng dữ liệu đã khai báo, sau đó DMA sẽ quay lại địa chỉ ban đầu truyền tiếp. Việc truyền cứ tiếp tục không dừng lại.
  
### 4. Cấu hình DMA

## II. PWM

### 1. Định nghĩa

- **PWM**(Pulse Width Mdulation) là điều khiến độ rộng xung.
  
- Trong điều khiển động cơ servo, tín hiệu PWM được sử dụng để chỉ định góc mà động cơ servo sẽ xoay đến (Nếu xung càng rộng thì servo càng gần góc 180 độ, xung càng hẹp thì vị trí servo càng về góc 0). Tín hiệu này có 2 yếu tố quan trọng:

    - Tần số: Là số lần tín hiệu lặp lại trong một giây. Đối với servo, tần số thông thường là 50Hz (tức là, chu kỳ lặp lại sau mỗi 20ms).

    - Độ rộng xung (Pulse Width): Là thời gian tín hiệu ở mức cao trong mỗi chu kỳ. Đo bằng microsecond (µs) và quyết định góc mà servo sẽ xoay đến. Tỉ lệ độ rộng xung với chu kì xung gọi là chu kì nhiệm vụ (Duty Cycle = Thời gian tín hiệu mức cao/ Tổng thời gian chu kỳ).
 
    - Góc 0 độ là khoảng 1000µs, góc 180 độ là khoảng 2000µs.

### 2. Cấu hình PWM


</details>

<details>
  <summary><h3>Bài 11: Flash & Bootloader</h3></summary>

Các bộ nhớ chính trong MCU: RAM, FLASH, EPROM

- RAM: Lưu biến, hàm tạm thời, có biến thay đổi nhiều -> Cần cập nhật liên tục nên tốc độ ghi/đọc nhanh, Mất nguồn mất dữ liệu.

- FLASH: Lưu chương trình Firmware, biến lưu giữa lâu dài: cấu hình ngoại vi,... -> tốc độ ghi chậm, đọc nhanh. Dữ liệu không bị mất khi mất điện.
    
    - Giới hạn số lần ghi (khoảng 100 nghìn lần).
   
    - Ghi 1 lần 2byte

- EPROM: Sử dụng 1 phần tương tự Flash để giả lập cho EPROM.

    - Khác FLASH là đọc/ghi theo từng byte.

## I. Flash
### 1. Định nghĩa

- Bộ nhớ Flash trong MCU là bộ nhớ dùng để lưu dữ liệu và chương trình cố định (chương trình firmware) có biến hoặc hàm lưu giữa lâu dài

- Tốc độ ghi chậm, tốc độ đọc nhanh. 

- Dữ liệu không bị mất khi mất nguồn (VD: Blink Led, sau khi nạp code vào MCU mất điện rồi cấp nguồn lại vẫn tiếp tục Blink).

- Giới hạn số lần ghi (khoảng 100 nghìn lần), trước khi ghi phải xóa Flash trước
   
- Chỉ có thể ghi theo khối, 1 lần 2/ 4byte.

#### Flash trong STM32F1xx

- STM32F1xx không có EPROM vật lý mà chỉ được cung cấp sẵn lấy một phần thông tin, dữ liệu của Flash 128/64kb.
 
- Kích thước của Flash: 1 Flash được chia làm nhiều Bank, vì STM32 chỉ có 64kb nên Flash chỉ có 1 Bank, Bank được chia nhỏ thành nhiều Page để dễ quản lý (kích thước 1kb)

- Để lập trình được 1 cái Page trong Flash, trước khi ghi cần phải xóa (Vì nó có cơ chế đưa từ bit 1 xuống 0, không thể đưa từ bit 0 lên 1). Mỗi lần xóa cả Page sẽ bị xóa trắng (0xFF).

- Khi ghi dữ liệu chỉ có thể ghi từ HalfWord (16 bit) đến 1 Word (32 bit). 

### 2. Cấu hình Flash



## II. Bootloader

### 1. Định nghĩa
Bootloader là một chương trình nhỏ nằm trong bộ nhớ của MCU, đóng vai trò khởi động và nạp chương trình chính (firmware) vào MCU. Chức năng: 

- Khi khởi động MCU: Bootloader là chương trình đầu tiên chạy đầu tiên, thực hiện các bước kiểm tra cơ bản. Gồm 2 loại Bootloader do nhà sản xuất viết và người dùng tự viết.

- Nạp firmware: Bootloader cho phép MCU nhận dữ liệu firmware từ các giao tiếp như SPI, UART nạp trực tiếp vào bộ nhớ flash.
  
▶️ Cập nhật firmware mà không cần thiết bị chuyên dụng(ST-link trong STM32)

#### Ưu điểm Bootloader?

- Cập nhật từ xa: Cho phép cập nhật firmware qua giao tiếp không dây (wifi, blutooth,..)

- Không cần thiết bị lập trình: Giúp nạp firmware mới mà không cần dùng đến các bộ lập trình chuyên dụng.

- Tiết kiệm thời gian & chi phí

### 2. Quá trình từ cấp nguồn hoặc reset đển hàm main()

#### Không có Bootloader

- **Xác định vị trí khởi động:**

    - Khi MCU được cấp nguồn hoặc reset, nó sẽ kiểm tra giá trị của các chân BOOT0 và BOOT1 để xác định vị trí bộ nhớ nào sẽ được sử dụng để khởi động.
     
    - Dựa trên giá trị của các chân này, MCU sẽ chọn một vùng nhớ để bắt đầu đọc lệnh.

- **Thiết lập MSP(Main Stack Pointer):**

    - Địa chỉ bắt đầu của vùng nhớ đó sẽ được lưu vào thanh ghi PC (Program Counter) để đọc lệnh, thanh ghi này sẽ xác định vị trí lệnh đầu tiên mà MCU cần thực thi.

    - Sau đó, lấy giá trị của ô nhớ đầu tiên để khởi tạo **MSP**(Main Stack Pointer) - Con 
trỏ chính của ngăn xếp.

- Thanh ghi PC sẽ chạy đến ô nhớ tiếp theo, ô nhớ này chứa địa chỉ của **Reset_Handler**.
 
- Cuối cùng, chương trình sẽ nhảy đến **Reset_Handler** để thực thi các nội dung trong hàm này:

    - Khởi tạo hệ thống: Cấp Clock, khởi tạo ngoại vi (phần cứng).
 
    - Sao chép dữ liệu (biến) từ Flash qua RAM: Để tính toán lập trình
 
    - Gọi hàm `main()`: Dưới hàm **Reset_Handler** là hàm `main()`.

#### Có Bootloader

- Sau khi Reset, MCU nhảy đến `Reset_Handler()` và **mặc định** nhảy đến chạy hàm `main()` của chương trình Bootloader.

- Chương trình Boot khởi tạo lại MSP bằng dữ liệu từ ô nhớ đầu tiên của nơi lưu chuowg trình Application.

- Gọi hàm **Bootloader()**, hàm này set trong thanh ghi **SCB_VTOR** theo địa chỉ của firmware Application (SCB_VTOR = Firmware address).

- MCU nhảy đến ô nhớ tiếp theo, là **Reset_Handler** của chương trình Application.

- Firmware mới bắt đầu chạy và MCU đã nhận diện **Reset_Handler** ở địa chỉ mới.

- Sau khi chuyển vùng địa chỉ, dù có nhấn nút Reset, thì MCU vẫn tiếp tục chạy trong Application.

</details>

<details>
  <summary><h3>Bài 12: CAN</h3></summary>

# Định nghĩa
Là hệ thống giao tiếp ....

## Tính chất

- Kết nối 2 bus xoắn lại

## Nguyên tắc hoạt động

- CAN không hoạt động Master-Slave, mà nó hoạt động Multi-Master(Master và Slave bình đẳng hoạt động như nhau).
  
▶️ Đôi khi xảy ra xung đột đường Bus, dựa vào nguyên tắc nào truyền trc nào sau (ID).

- CAN không có địa chỉ, muốn truyền đến Node CAN nào thì dựa vào ID (Mục đính, Comment,...)
  
▶️ Mỗi ID đều có thông điệp khác, không trùng lặp.

- Tất cả các Node trên 1 đường Bus đều được nhận thông điệp truyền đi, nên dựa vào ID sẽ thực hiện cơ chế kiểm tra xem thông điệp có phụ hợp để hoạt động hay không (VD: màn hình ô tô, gương chiếu hậu,...)
  
▶️ Nếu thông điệp có liên qua nó được xử lý, không thì bỏ qua.

## Tranh chấp trên BUS

- CAN cho phép các Nút khác nhau gửi dữ liệu cùng lúc, mà dữ liệu rất rất

▶️ Phương pháp, giải quyết tranh chấp 
(Bit 0: Ưu tiên cao hơn; Bit 1: Ưu tiên thấp hơn)

## CAN Frame
(CAN 2.0A, CAN 2.0B, CAN 3.0 FD)

- **Data Frame:** Gồm 2 loại Standard frame &

- **Remote Frame**

- **Error Frame:** Gồm 5 lỗi chính
     - Bit Error
 
     - CRC Error
 
     - ACK Error: node nhận không nhận ACK
 
     - Form Error:
 
     - Stuff Error: Nếu 5 bít giống nhau, thì bit thứ 6 phải khác. Nếu bit thứ 6 giống thì báo lỗi này.

### 1. Cấu trúc Data Frame
(phút 40)
- Start of frame(SOF)

- Arbitration Field

- Control Field

- Data Field

- CRC Field: Chứa biểu thức để check lỗi

- ACK Field (hoạt động giống UART): Chứa Bit phản hồi

- End of Frame

### 2. Remote Frame
(Cảm biến áp suất lốp)- phút 45

### 3. Error Frame

Trong giao thức CAN có 2 loại error frame: 

- Active Error Frame: Mất tín hiệu

- Passive Error Frame(Error Counter): Lỗi nghiêm trọng 

## Overload Frame(Khung quá tải)


</details>



