CC=gcc
CFLAGS=-ffunction-sections -fdata-sections 
LDFLAGS=-Wl,--gc-sections 


# fill in all your make rules
vm_x2017: vm_x2017.c functions_for_objdump.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
	strip vm_x2017
	./upx vm_x2017


objdump_x2017: objdump_x2017.c functions_for_objdump.c 
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

tests:
	bash test.sh

run_tests:
	bash test.sh

clean:
	rm objdump_x2017
	rm vm_x2017

	


