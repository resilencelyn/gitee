# 基于riscv64处理器的QEMU平台支持

# TODO

- [x] 添加异常处理
  - [x] 时钟中断导致异常，换成opensbi再试试。
- [x] 添加内存管理
- [x] 添加时钟中断
- [x] 添加中断管理，中断控制器
- [x] 添加串口接收中断
- [x] 添加原子操作
- [x] 添加上下文切换操作
- [x] 完善多线程下上下文切换操作
- [x] 添加quickstart.md
- [ ] 添加内核MMU映射

- [ ] 添加SMP支持
  - [x] 添加SMP启动
  - [ ] 添加SMP调度
    - [ ] 线程sleep有BUG。
    - [ ] SpinLockIRQ中添加level支持
    
  

# 内存布局

see https://nankai.gitbook.io/ucore-os-on-risc-v64/lab2/wu-li-nei-cun-tan-ce

