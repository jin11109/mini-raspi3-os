#ifndef _BOOTLOADER_MINI_UART_H
#define _BOOTLOADER_MINI_UART_H

void mini_uart_init(void);

/* Blocking write a character */
void mini_uart_sync_write(const char c);
/* Blocking write a string */
void mini_uart_sync_write_str(const char* str);
/* Blocking read a character */
char mini_uart_sync_read(void);

#endif /* _BOOTLOADER_MINI_UART_H */
