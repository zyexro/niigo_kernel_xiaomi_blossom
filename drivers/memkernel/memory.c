#include "memory.h"
#include <linux/tty.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#include <asm/cpu.h>
#include <asm/io.h>
#include <asm/page.h>
#include <asm/pgtable.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
#include <linux/sched/mm.h>
#include <linux/sched/task.h>
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0))
#include <linux/mmap_lock.h>
#define MM_READ_LOCK(mm) mmap_read_lock(mm);
#define MM_READ_UNLOCK(mm) mmap_read_unlock(mm);
#else
#include <linux/rwsem.h>
#define MM_READ_LOCK(mm) down_read(&(mm)->mmap_sem);
#define MM_READ_UNLOCK(mm) up_read(&(mm)->mmap_sem);
#endif


phys_addr_t translate_linear_address(struct mm_struct *mm, uintptr_t va)
{
	pgd_t *pgd;
#ifdef p4d_none
	p4d_t *p4d;
#endif
	pmd_t *pmd;
	pte_t *pte;
	pud_t *pud;

	phys_addr_t page_addr;
	uintptr_t page_offset;

	pgd = pgd_offset(mm, va);
	if (pgd_none(*pgd) || pgd_bad(*pgd)) {
		return 0;
	}
#ifdef p4d_none
	p4d = p4d_offset(pgd, va);
	if (p4d_none(*p4d) || p4d_bad(*p4d)) {
		return 0;
	}
	pud = pud_offset(p4d, va);
#else
    pud = pud_offset(pgd, va);
#endif
	if (pud_none(*pud) || pud_bad(*pud)) {
		return 0;
	}
	pmd = pmd_offset(pud, va);
	if (pmd_none(*pmd)) {
		return 0;
	}
	pte = pte_offset_kernel(pmd, va);
	if (pte_none(*pte)) {
		return 0;
	}
	if (!pte_present(*pte)) {
		return 0;
	}
	page_addr = (phys_addr_t)(pte_pfn(*pte) << PAGE_SHIFT);
	page_offset = va & (PAGE_SIZE - 1);

	return page_addr + page_offset;
}

#if !defined(ARCH_HAS_VALID_PHYS_ADDR_RANGE) || defined(MODULE)
static inline int memk_valid_phys_addr_range(phys_addr_t addr, size_t size)
{
	return addr + size <= __pa(high_memory);
}
#define IS_VALID_PHYS_ADDR_RANGE(x,y) memk_valid_phys_addr_range(x,y)
#else
#define IS_VALID_PHYS_ADDR_RANGE(x,y) valid_phys_addr_range(x,y)
#endif

bool read_physical_address(phys_addr_t pa, void *buffer, size_t size)
{
	void *mapped;

	if (!pfn_valid(__phys_to_pfn(pa))) {
		return false;
	}
	if (!IS_VALID_PHYS_ADDR_RANGE(pa, size)) {
		return false;
	}
	mapped = ioremap_cache(pa, size);
	if (!mapped) {
		return false;
	}
	if (copy_to_user(buffer, mapped, size)) {
		iounmap(mapped);
		return false;
	}
	iounmap(mapped);
	return true;
}

bool write_physical_address(phys_addr_t pa, void *buffer, size_t size)
{
	void *mapped;

	if (!pfn_valid(__phys_to_pfn(pa))) {
		return false;
	}
	if (!IS_VALID_PHYS_ADDR_RANGE(pa, size)) {
		return false;
	}
	mapped = ioremap_cache(pa, size);
	if (!mapped) {
		return false;
	}
	if (copy_from_user(mapped, buffer, size)) {
		iounmap(mapped);
		return false;
	}
	iounmap(mapped);
	return true;
}

bool read_process_memory(
	pid_t pid,
	uintptr_t addr,
	void *buffer,
	size_t size)
{

	struct task_struct *task;
	struct mm_struct *mm;
	struct pid *pid_struct;
	phys_addr_t pa;

	pid_struct = find_get_pid(pid);
	if (!pid_struct) {
		return false;
	}
	task = get_pid_task(pid_struct, PIDTYPE_PID);
	put_pid(pid_struct);
	if (!task) {
		return false;
	}
	mm = get_task_mm(task);
	put_task_struct(task);
	if (!mm) {
		return false;
	}
	MM_READ_LOCK(mm);
	pa = translate_linear_address(mm, addr);
	MM_READ_UNLOCK(mm);
	mmput(mm);
	if (!pa) {
		return false;
	}

	return read_physical_address(pa, buffer, size);
}

bool write_process_memory(
	pid_t pid,
	uintptr_t addr,
	void *buffer,
	size_t size)
{

	struct task_struct *task;
	struct mm_struct *mm;
	struct pid *pid_struct;
	phys_addr_t pa;

	pid_struct = find_get_pid(pid);
	if (!pid_struct) {
		return false;
	}
	task = get_pid_task(pid_struct, PIDTYPE_PID);
	put_pid(pid_struct);
	if (!task) {
		return false;
	}
	mm = get_task_mm(task);
	put_task_struct(task);
	if (!mm) {
		return false;
	}
	MM_READ_LOCK(mm);
	pa = translate_linear_address(mm, addr);
	MM_READ_UNLOCK(mm);
	mmput(mm);
	if (!pa) {
		return false;
	}

	return write_physical_address(pa, buffer, size);
}
