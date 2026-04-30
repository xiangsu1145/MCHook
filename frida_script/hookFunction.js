// 等待模块加载（Windows 下为 .exe 或 .dll）
function waitForModule(moduleName, callback) {
    const module = Process.getModuleByName(moduleName);
    if (module) {
        callback(module);
    } else {
        console.log(`等待模块 ${moduleName} 加载...`);
        setTimeout(() => waitForModule(moduleName, callback), 500);
    }
}

// 替换为实际的模块名（可以是主程序或某个 DLL）
const TARGET_MODULE = "Minecraft.Windows.exe";
// 替换为真实的函数偏移（需通过逆向获取）
const FUNCTION_OFFSET = 0x5D64C70;  // 示例偏移，请根据实际情况修改

waitForModule(TARGET_MODULE, function (module) {
    console.log(`模块已加载: ${module.name} (基址: ${module.base})`);

    // 计算函数绝对地址
    const functionAddress = module.base.add(FUNCTION_OFFSET);
    console.log(`目标函数地址: ${functionAddress}`);

    // 开始 Hook
    Interceptor.attach(functionAddress, {
        onEnter: function (args) {
            console.log("\n========================================");
            console.log(`[+] ${TARGET_MODULE} 中的目标函数被调用`);
            console.log(`调用地址: ${functionAddress}`);

            // 打印参数（根据实际函数原型，这里是示例）
            console.log("参数列表:");
            for (let i = 0; i < 2; i++) { // 通常打印前4个参数，可根据需要调整
                if (args[i] && !args[i].isNull()) {
                    console.log(`  arg[${i}] = ${args[i]}`);
                }
            }

            // 打印调用堆栈
            console.log("调用堆栈 (从当前函数向上回溯):");
            try {
                const backtrace = Thread.backtrace(this.context, Backtracer.ACCURATE);
                backtrace.forEach((addr, idx) => {
                    const moduleInfo = Process.findModuleByAddress(addr);
                    if (moduleInfo) {
                        const offset = addr.sub(moduleInfo.base);
                        console.log(`  #${idx}: ${addr} | ${moduleInfo.name} + 0x${offset.toString(16)}`);
                    } else {
                        console.log(`  #${idx}: ${addr} | [未知模块]`);
                    }
                });
            } catch (e) {
                console.log(`  [堆栈获取失败] ${e.message}`);
            }
        },
        onLeave: function (retval) {
            console.log(`返回值: ${retval}`);
            console.log("========================================\n");
        }
    });

    console.log(`[*] Hook 已安装，等待函数被触发...`);
});