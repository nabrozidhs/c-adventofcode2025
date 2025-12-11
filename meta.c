#include <stdio.h>

const int MAX_DAY = 11;

int main(int argc, char **argv)
{
    FILE* f = fopen("days.c", "w");
    if (f == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        return -1;
    }

    fprintf(f, "// Generated code do not edit!\n");
    fprintf(f, "\n");
    for (int day = 1; day <= MAX_DAY; ++day)
    {
        fprintf(f, "#include \"day%02d.c\"\n", day);
    }
    fprintf(f, "\n");
    fprintf(f, "void run_all(MemoryArena *arena) {\n");
    for (int day = 1; day <= MAX_DAY; ++day)
    {
        fprintf(f, "  {\n");
        fprintf(f, "    memory_arena_reset(arena);\n");
        fprintf(f, "    DayInput day_input = day_input_from_file(\"day%02d.txt\", arena);\n", day);
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
