#include <stdio.h>

#define MIN_DAY 1
#define MAX_DAY 11

int main(void)
{
    FILE* f = fopen("days.c", "w");
    if (f == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        return -1;
    }

    fprintf(f, "// Generated code do not edit!\n");
    fprintf(f, "\n");
    for (int day = MIN_DAY; day <= MAX_DAY; ++day)
    {
        fprintf(f, "#include \"day%02d.c\"\n", day);
    }
    fprintf(f, "\n");
    fprintf(f, "static void run_all(MemoryArena *arena) {\n");
    fprintf(f, "  DayInput day_input = {0};\n");
    for (int day = MIN_DAY; day <= MAX_DAY; ++day)
    {
        fprintf(f, "  {\n");
        fprintf(f, "    memory_arena_reset(arena);\n");
        fprintf(f, "    day_input = day_input_from_file(\"day%02d.txt\", arena);\n", day);
        fprintf(f, "    printf(\"Running day %d\\n\");\n", day);
        fprintf(f, "    day%02d(&day_input, arena);\n", day);
        fprintf(f, "  }\n");

        if (day != MAX_DAY)
        {
            fprintf(f, "\n");
        }
    }
    fprintf(f, "}\n");

    if (fclose(f))
    {
        fprintf(stderr, "Error closing file\n");
        return -1;
    }

    return 0;
}
