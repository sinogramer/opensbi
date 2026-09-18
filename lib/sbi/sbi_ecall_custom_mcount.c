#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_ecall.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_trap.h>
#include <sbi/sbi_types.h>  // struct sbi_ecall_return

#define EXTID_MYECALL 0x09000000
#define FID_WRITE_INHIBIT   0
#define FID_WRITE_ENABLE    1
#define FID_WRITE_BOTH      2

static int my_handler(unsigned long extid, 
                      unsigned long fid,
                      struct sbi_trap_regs *regs,
                      struct sbi_ecall_return *out)
{
    unsigned long value = 0;
    out->skip_regs_update = false;
    out->value = 0;

    switch (fid) {
        case FID_WRITE_INHIBIT:
            value = regs->a0;
            __asm__ __volatile__("csrw mcountinhibit, %0" :: "r"(value));
            sbi_printf("[OpenSBI] Set mcountinhibit = 0x%lx\n", value);

            return SBI_SUCCESS;
        
        case FID_WRITE_ENABLE:
            value = regs->a1;
            __asm__ __volatile__("csrw mcounteren, %0" :: "r"(value));
            sbi_printf("[OpenSBI] Set mcounteren = 0x%lx\n", value);

            return SBI_SUCCESS;

        case FID_WRITE_BOTH:
            value = regs->a0;
            __asm__ __volatile__("csrw mcountinhibit, %0" :: "r"(value));
            sbi_printf("[OpenSBI] Set mcountinhibit = 0x%lx\n", value);

            value = regs->a1;
            __asm__ __volatile__("csrw mcounteren, %0" :: "r"(value));
            sbi_printf("[OpenSBI] Set mcounteren = 0x%lx\n", value);

            return SBI_SUCCESS;
        
        default:
            out->value = SBI_ERR_NOT_SUPPORTED;
            return 0;
    }
}

struct sbi_ecall_extension sbi_ecall_custom_mcount = {
    .extid_start = EXTID_MYECALL,
    .extid_end   = EXTID_MYECALL,
    .handle      = my_handler,
};

int sbi_ecall_custom_mcount_register_extensions(void)
{
    return sbi_ecall_register_extension(&sbi_ecall_custom_mcount);
}
