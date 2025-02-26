/*
 * x86 specific code for irq_work
 *
 * Copyright (C) 2010 Red Hat, Inc., Peter Zijlstra
 */

#include <linux/kernel.h>
#include <linux/irq_work.h>
#include <linux/hardirq.h>
#include <asm/apic.h>
#include <linux/interrupt.h>

static inline void __smp_irq_work_interrupt(void)
{
	inc_irq_stat(apic_irq_work_irqs);
	irq_work_run();
}

__visible void __irq_entry smp_irq_work_interrupt(struct pt_regs *regs)
{
	ipi_entering_ack_irq();
	__smp_irq_work_interrupt();
	exiting_irq();
}

__visible void __irq_entry smp_trace_irq_work_interrupt(struct pt_regs *regs)
{
	ipi_entering_ack_irq();
	__smp_irq_work_interrupt();
	exiting_irq();
}

void arch_irq_work_raise(void)
{
#ifdef CONFIG_X86_LOCAL_APIC
	if (!arch_irq_work_has_interrupt())
		return;

	apic->send_IPI_self(IRQ_WORK_VECTOR);
	apic_wait_icr_idle();
#endif
}
