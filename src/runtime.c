// this is the native host that runs the host + cart. The same thing is also implemented in docs/index.html for web.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bh_read_file.h"
#include "platform_common.h"
#include "wasm_export.h"

static char *module_search_path = ".";
static bool
module_reader_callback(package_type_t module_type, const char *module_name,
                       uint8 **p_buffer, uint32 *p_size)
{
    char *file_format = NULL;
#if WASM_ENABLE_INTERP != 0
    if (module_type == Wasm_Module_Bytecode)
        file_format = ".wasm";
#endif
#if WASM_ENABLE_AOT != 0
    if (module_type == Wasm_Module_AoT)
        file_format = ".aot";

#endif
    bh_assert(file_format != NULL);
    const char *format = "%s/%s%s";
    int sz = strlen(module_search_path) + strlen("/") + strlen(module_name)
             + strlen(file_format) + 1;
    char *wasm_file_name = wasm_runtime_malloc(sz);
    if (!wasm_file_name) {
        return false;
    }
    snprintf(wasm_file_name, sz, format, module_search_path, module_name,
             file_format);
    *p_buffer = (uint8_t *)bh_read_file_to_buffer(wasm_file_name, p_size);

    wasm_runtime_free(wasm_file_name);
    return *p_buffer != NULL;
}

static void
module_destroyer_callback(uint8 *buffer, uint32 size)
{
    if (!buffer) {
        return;
    }

    wasm_runtime_free(buffer);
    buffer = NULL;
}

// 10M
static char sandbox_memory_space[10 * 1024 * 1024] = { 0 };

int main(int argc, char *argv[]) {
    bool ret = false;
    if (argc != 2) {
        return -1;
    }
    char *wasm_file = argv[1];
    // 16K
    const uint32 stack_size = 16 * 1024;
    const uint32 heap_size = 16 * 1024;

    RuntimeInitArgs init_args = { 0 };
    char error_buf[128] = { 0 };
    // parameters and return values
    char *args[1] = { 0 };

    uint8 *file_buf = NULL;
    uint32 file_buf_size = 0;
    wasm_module_t module = NULL;
    wasm_module_t module1;
    wasm_module_inst_t module_inst = NULL;

    // all malloc() only from the given buffer
    init_args.mem_alloc_type = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf = sandbox_memory_space;
    init_args.mem_alloc_option.pool.heap_size = sizeof(sandbox_memory_space);

    printf("- wasm_runtime_full_init\n");
    // initialize runtime environment
    if (!wasm_runtime_full_init(&init_args)) {
      printf("Init runtime environment failed.\n");
      goto EXIT;
    }

    wasm_runtime_set_module_reader(module_reader_callback, module_destroyer_callback);

    // load WASM byte buffer from WASM bin file
    if (!(file_buf = (uint8 *)bh_read_file_to_buffer(wasm_file, &file_buf_size))) {
      goto RELEASE_RUNTIME;
    }
    // load cart and let WAMR load host
    printf("- wasm_runtime_load\n");

    if (!(module = wasm_runtime_load(file_buf, file_buf_size, error_buf, sizeof(error_buf)))) {
      printf("%s\n", error_buf);
      goto RELEASE_BINARY;
    }

    // instantiate the module
    printf("- wasm_runtime_instantiate\n");
    if (!(module_inst = wasm_runtime_instantiate(
              module, stack_size, heap_size, error_buf, sizeof(error_buf)))) {
        printf("%s\n", error_buf);
        goto UNLOAD_MODULE;
    }

    wasm_application_execute_func(module_inst, "cart_load", 0, args);
    wasm_application_execute_func(module_inst, "cart_update", 0, args);

    ret = true;

    printf("- wasm_runtime_deinstantiate\n");
    wasm_runtime_deinstantiate(module_inst);
UNLOAD_MODULE:
    printf("- wasm_runtime_unload\n");
    wasm_runtime_unload(module);
RELEASE_BINARY:
    module_destroyer_callback(file_buf, file_buf_size);
RELEASE_RUNTIME:
    printf("- wasm_runtime_destroy\n");
    wasm_runtime_destroy();
EXIT:
    return ret ? 0 : 1;
}