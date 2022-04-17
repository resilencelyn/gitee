import { FileType } from '../src/IVsshStatus';
import VirtualFileSystem from '../src/VirtualFileSystem';

test("测试虚拟文件系统",()=>{
    let vm = new VirtualFileSystem();
    expect(vm.getCurrentPath()).toBe("/")
    vm.addVirtualPath("/home/iven",FileType.folder);
    vm.cd("/home");
    expect(vm.getCurrentPath()).toBe("/home")
    
})