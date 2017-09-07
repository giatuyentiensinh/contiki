## Exercise

### Exercise 1
Nạp chương trình exercise1 bằng lệnh

```
$ make exercise1.upload
```

### Exercise 2

- Nạp chương trình border-router vào RE-Mote đóng vai trò là border-router

 ```
 $ cd border-router
 $ make border-router.upload MOTES=/dev/ttyUSB0 && make connect-router PREFIX="bbbb::1/64"
 ```
- Nạp chương trình client vào RE-Mote đóng vai trò là client.

 ```
 $ make exercise2.upload MOTES=/dev/ttyUSB1 && make login MOTES=/dev/ttyUSB1
 ```

- Chạy chương trình UDPServer trên máy tính nối với border-router.

 ```
 $ javac javacode/*.java && java javacode.UDPServer 

 ```

- Cắm sensor `DHT22` vào RE-Mote đóng vai trò là client với chân tín hiệu của sensor ứng với PIN PA5 của RE-Mote. 

### Exercise 3

- Nạp chương trình border-router vào RE-Mote đóng vai trò là border-router

 ```
 $ cd border-router
 $ make border-router.upload MOTES=/dev/ttyUSB0 && make connect-router PREFIX="bbbb::1/64"
 ```

- Nạp chương trình coap server vào RE-Mote đóng vai trò là coap server.

 ```
 $ make exercise3.upload MOTES=/dev/ttyUSB1
 ```
 
- Cắm sensor `DHT22` vào RE-Mote đóng vai trò là client với chân tín hiệu của sensor ứng với PIN PA5 của RE-Mote.

- Mở trình duyệt và nhập địa chỉ IPv6 của border-router để lấy địa chỉ IPv6 của node chạy coap server. eg: `http://[bbbb::212:4b00:9df:4f53]`. Sau đó gõ địa chỉ của coap server trên trình duyệt firefox (đã có Extension Copper). eg: `coap://[bbbb::212:4b00:615:a974]:5683/iot/contiki-remote`. Tiếp đó enable phần `Debug Control` và trong phần `Accept` gõ `application/json`. Cuối cùng nhấn button `GET` để kiểm tra kết quả.

### Exercise 4

- Nạp chương trình border-router vào RE-Mote đóng vai trò là border-router

 ```
 $ cd border-router
 $ make border-router.upload MOTES=/dev/ttyUSB0 && make connect-router PREFIX="cccc::1/64"
 ```
- Nạp chương trình client vào 2 RE-Mote đóng vai trò là client.
 
 ```
 $ make exercise2.upload MOTES=/dev/ttyUSB1 && make login MOTES=/dev/ttyUSB1
 $ make exercise2.upload MOTES=/dev/ttyUSB2 && make login MOTES=/dev/ttyUSB2
 ```

- Thiết lập topology đa chặng như bài lesson 4 trong tài liệu.
- Chạy chương trình UDPServer trên máy tính nối với border-router.

 ```
 $ javac javacode/*.java && java javacode.UDPServer 

 ```