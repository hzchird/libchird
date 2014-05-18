int doSystem(char * fmt, ...)
{
	char cmd[1024];
	va_list ap;
	int ret;
	
	va_start(ap, fmt);
	vsprintf(cmd, fmt, ap);
	va_end(ap);

	ret = system(cmd);
	
	return ret;
}