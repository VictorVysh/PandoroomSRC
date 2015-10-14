#define F_CPU 8000000

void lcd_write_cmd(int data);
void lcd_init();
void lcd_write_char(char ch);
void lcd_write_str(char*str);
void lcd_set_cursor(int line,int pos);