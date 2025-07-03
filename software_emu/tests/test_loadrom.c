
#include <machdep/cart.h>
#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILE_SIZE  8 * 1024 * 1024


START_TEST(load_rom_test)
{
    char path[200] = "test_roms/tetris.gb"; 
    uint8_t rom_buf[MAX_FILE_SIZE];
    size_t filesize;

    FILE *romfile = fopen(path, 'rb');

    // Find length of file
    fseek(romfile, 0, SEEK_SET);
    filesize = ftell(romfile);
    rewind(romfile);

    ck_assert_msg(romfile <= MAX_FILE_SIZE, "File too big!");

    size_t readsize = fread(rom_buf, MAX_FILE_SIZE, 1, romfile);

    ck_assert_msg(filesize == readsize, "Error reading file");

        
    










}
END_TEST




Suite *loadrom_suit(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("LoadROM");
    tc_core = tcase_create("");

    tcase_add_test(tc_core, load_rom_test);
    
    suite_add_tcase(s, tc_core);
    return s;
}


int main(void)
{
    int num_failed;
    Suite *s;
    SRunner *sr;

    s = loadrom_suit();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    num_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (num_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}



