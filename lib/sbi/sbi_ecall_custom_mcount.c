#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_ecall.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_trap.h>
#include <sbi/sbi_types.h>  // struct sbi_ecall_return

#define EXTID_MYECALL 0x09000000

#define FID_WRITE_INHIBIT   0
#define FID_WRITE_ENABLE    1
#define FID_WRITE_BOTH      2

#define FID_READ_INHIBIT    3
#define FID_READ_ENABLE     4
#define FID_READ_BOTH       5

#define FID_SET_INHIBIT     6
#define FID_SET_ENABLE      7
#define FID_SET_BOTH        8

#define FID_CLEAR_INHIBIT   9
#define FID_CLEAR_ENABLE    10
#define FID_CLEAR_BOTH      11


static int my_handler(unsigned long extid, 
                      unsigned long fid,
                      struct sbi_trap_regs *regs,
                      struct sbi_ecall_return *out)
{
    unsigned long value = 0;
    unsigned long old_value = 0;
    out->skip_regs_update = false;
    out->value = 0;

    switch (fid) {
        case FID_WRITE_INHIBIT:
            value = regs->a0;
            __asm__ __volatile__(
                "csrrw %0, mcountinhibit, %1" 
                : "=r"(old_value)
                : "r"(value)
            );
            sbi_printf("[OpenSBI] Write mcountinhibit = 0x%lx (old: 0x%lx)\n", value, old_value);
            break;
        
        case FID_WRITE_ENABLE:
            value = regs->a1;
            __asm__ __volatile__(
                "csrrw %0, mcounteren, %1" 
                : "=r"(old_value)
                : "r"(value)
            );
            sbi_printf("[OpenSBI] Write mcounteren = 0x%lx (old: 0x%lx)\n", value, old_value);
            break;

        case FID_WRITE_BOTH:
            value = regs->a0;
            __asm__ __volatile__(
                "csrrw %0, mcountinhibit, %1" 
                : "=r"(old_value)
                : "r"(value)
            );
            sbi_printf("[OpenSBI] Write mcountinhibit = 0x%lx (old: 0x%lx)\n", value, old_value);
            value = regs->a1;
            
            __asm__ __volatile__(
                "csrrw %0, mcounteren, %1" 
                : "=r"(old_value)
                : "r"(value)
            );
            sbi_printf("[OpenSBI] Write mcounteren = 0x%lx (old: 0x%lx)\n", value, old_value);
            old_value = 0; // Reset old_value to avoid confusion in the output
            break;
        
        case FID_READ_INHIBIT:
            __asm__ __volatile__("csrr %0, mcountinhibit" : "=r"(old_value));
            sbi_printf("[OpenSBI] Read mcountinhibit = 0x%lx\n", old_value);
            break;
        
        case FID_READ_ENABLE:
            __asm__ __volatile__("csrr %0, mcounteren" : "=r"(old_value));
            sbi_printf("[OpenSBI] Read mcounteren = 0x%lx\n", old_value);
            break;

        case FID_READ_BOTH:
            __asm__ __volatile__("csrr %0, mcountinhibit" : "=r"(old_value));
            sbi_printf("[OpenSBI] Read mcountinhibit = 0x%lx\n", old_value);

            __asm__ __volatile__("csrr %0, mcounteren" : "=r"(old_value));
            sbi_printf("[OpenSBI] Read mcounteren = 0x%lx\n", old_value);
            old_value = 0; // Reset old_value to avoid confusion in the output
            break;

        case FID_SET_INHIBIT:
            value = regs->a0;
            __asm__ __volatile__(
                "csrrs %0, mcountinhibit, %1" 
                : "=r"(old_value)
                : "r"(value));
            sbi_printf("[OpenSBI] Set mcountinhibit = 0x%lx (old: 0x%lx, new: 0x%lx)\n", value, old_value, value | old_value);
            break;
        
        case FID_SET_ENABLE:
            value = regs->a1;
            __asm__ __volatile__(
                "csrrs %0, mcounteren, %1" 
                : "=r"(old_value)
                : "r"(value));
            sbi_printf("[OpenSBI] Set mcounteren = 0x%lx (old: 0x%lx, new: 0x%lx)\n", value, old_value, value | old_value);
            break;

        case FID_SET_BOTH:
            value = regs->a0;
            __asm__ __volatile__(
                "csrrs %0, mcountinhibit, %1" 
                : "=r"(old_value)
                : "r"(value));
            sbi_printf("[OpenSBI] Set mcountinhibit = 0x%lx (old: 0x%lx, new: 0x%lx)\n", value, old_value, value | old_value);

            value = regs->a1;
            __asm__ __volatile__(
                "csrrs %0, mcounteren, %1" 
                : "=r"(old_value)
                : "r"(value));
            sbi_printf("[OpenSBI] Set mcounteren = 0x%lx (old: 0x%lx, new: 0x%lx)\n", value, old_value, value | old_value);
            old_value = 0; // Reset old_value to avoid confusion in the output
            break;

        case FID_CLEAR_INHIBIT:
            value = regs->a0;
            __asm__ __volatile__(
                "csrrc %0, mcountinhibit, %1" 
                : "=r"(old_value)
                : "r"(value));
            sbi_printf("[OpenSBI] Clear mcountinhibit = 0x%lx (old: 0x%lx, new: 0x%lx)\n", value, old_value, old_value & ~value);
            break;

        case FID_CLEAR_ENABLE:
            value = regs->a1;
            __asm__ __volatile__(
                "csrrc %0, mcounteren, %1" 
                : "=r"(old_value)
                : "r"(value));
            sbi_printf("[OpenSBI] Clear mcounteren = 0x%lx (old: 0x%lx, new: 0x%lx)\n", value, old_value, old_value & ~value);
            break;

        case FID_CLEAR_BOTH:
            value = regs->a0;
            __asm__ __volatile__(
                "csrrc %0, mcountinhibit, %1" 
                : "=r"(old_value)
                : "r"(value));
            sbi_printf("[OpenSBI] Clear mcountinhibit = 0x%lx (old: 0x%lx, new: 0x%lx)\n", value, old_value, old_value & ~value);

            value = regs->a1;
            __asm__ __volatile__(
                "csrrc %0, mcounteren, %1" 
                : "=r"(old_value)
                : "r"(value));
            sbi_printf("[OpenSBI] Clear mcounteren = 0x%lx (old: 0x%lx, new: 0x%lx)\n", value, old_value, old_value & ~value);
            old_value = 0; // Reset old_value to avoid confusion in the output
            break;
        
        default:
            return SBI_ERR_NOT_SUPPORTED;
    }

    out->value = old_value;

    return SBI_SUCCESS;
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
