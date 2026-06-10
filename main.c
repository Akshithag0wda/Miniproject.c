#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define HEIGHT 20
#define WIDTH 50
#define MAX_SHAPES 100

// Shape types
typedef enum {
    SHAPE_LINE,
    SHAPE_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_TRIANGLE
} ShapeType;

// Parameter structures for each shape
typedef struct {
    int x1, y1;
    int x2, y2;
} LineParams;

typedef struct {
    int x1, y1;
    int x2, y2;
} RectParams;

typedef struct {
    int xc, yc;
    int r;
} CircleParams;

typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3;
} TriangleParams;

// Combined Shape structure
typedef struct {
    ShapeType type;
    union {
        LineParams line;
        RectParams rect;
        CircleParams circle;
        TriangleParams triangle;
    } params;
} Shape;

// Global state
Shape shapes[MAX_SHAPES];
int num_shapes = 0;
char canvas[HEIGHT][WIDTH];

// Safely clear stdin input buffer to prevent infinite loops
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Get standard integer input within bounds
int get_int_input(const char* prompt, int min_val, int max_val) {
    int val;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &val) == 1) {
            clear_input_buffer();
            if (val >= min_val && val <= max_val) {
                return val;
            }
            printf("Invalid range! Please enter a value between %d and %d.\n", min_val, max_val);
        } else {
            clear_input_buffer();
            printf("Invalid input! Please enter an integer.\n");
        }
    }
}

// Canvas manipulation functions
void clear_canvas(char canvas[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            canvas[y][x] = '_';
        }
    }
}

// Safely draw pixel inside boundaries
void set_pixel(char canvas[HEIGHT][WIDTH], int x, int y) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        canvas[y][x] = '*';
    }
}

// Bresenham's Line Algorithm
void draw_line(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = -abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (1) {
        set_pixel(canvas, x1, y1);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// Draw rectangle outlines
void draw_rectangle(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2) {
    int start_x = x1 < x2 ? x1 : x2;
    int end_x = x1 < x2 ? x2 : x1;
    int start_y = y1 < y2 ? y1 : y2;
    int end_y = y1 < y2 ? y2 : y1;

    // Draw horizontal lines
    for (int x = start_x; x <= end_x; x++) {
        set_pixel(canvas, x, start_y);
        set_pixel(canvas, x, end_y);
    }
    // Draw vertical lines
    for (int y = start_y; y <= end_y; y++) {
        set_pixel(canvas, start_x, y);
        set_pixel(canvas, end_x, y);
    }
}

// Draw 8 symmetric points for standard Circle Algorithm
void draw_circle_points(char canvas[HEIGHT][WIDTH], int xc, int yc, int x, int y) {
    set_pixel(canvas, xc + x, yc + y);
    set_pixel(canvas, xc - x, yc + y);
    set_pixel(canvas, xc + x, yc - y);
    set_pixel(canvas, xc - x, yc - y);
    set_pixel(canvas, xc + y, yc + x);
    set_pixel(canvas, xc - y, yc + x);
    set_pixel(canvas, xc + y, yc - x);
    set_pixel(canvas, xc - y, yc - x);
}

// Bresenham's Midpoint Circle Algorithm
void draw_circle(char canvas[HEIGHT][WIDTH], int xc, int yc, int r) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    draw_circle_points(canvas, xc, yc, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        draw_circle_points(canvas, xc, yc, x, y);
    }
}

// Draw triangle from 3 vertices using lines
void draw_triangle(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(canvas, x1, y1, x2, y2);
    draw_line(canvas, x2, y2, x3, y3);
    draw_line(canvas, x3, y3, x1, y1);
}

// Re-render all shapes on clean canvas
void render_canvas() {
    clear_canvas(canvas);
    for (int i = 0; i < num_shapes; i++) {
        Shape s = shapes[i];
        switch (s.type) {
            case SHAPE_LINE:
                draw_line(canvas, s.params.line.x1, s.params.line.y1, s.params.line.x2, s.params.line.y2);
                break;
            case SHAPE_RECTANGLE:
                draw_rectangle(canvas, s.params.rect.x1, s.params.rect.y1, s.params.rect.x2, s.params.rect.y2);
                break;
            case SHAPE_CIRCLE:
                draw_circle(canvas, s.params.circle.xc, s.params.circle.yc, s.params.circle.r);
                break;
            case SHAPE_TRIANGLE:
                draw_triangle(canvas, s.params.triangle.x1, s.params.triangle.y1,
                                      s.params.triangle.x2, s.params.triangle.y2,
                                      s.params.triangle.x3, s.params.triangle.y3);
                break;
        }
    }
}

// Print canvas grid with horizontal and vertical headers for easy coordinate tracing
void display_canvas_with_borders() {
    printf("\n");
    // Top border
    printf("   +");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("+\n");

    for (int y = 0; y < HEIGHT; y++) {
        printf("%2d |", y);
        for (int x = 0; x < WIDTH; x++) {
            printf("%c", canvas[y][x]);
        }
        printf("|\n");
    }

    // Bottom border
    printf("   +");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("+\n");
    
    // Draw column numbers (tens)
    printf("    ");
    for (int x = 0; x < WIDTH; x++) {
        if (x % 10 == 0) printf("%d", x / 10);
        else printf(" ");
    }
    printf("\n    ");
    // Draw column numbers (units)
    for (int x = 0; x < WIDTH; x++) {
        printf("%d", x % 10);
    }
    printf("\n\n");
}

// List all shapes currently drawn
void list_shapes() {
    if (num_shapes == 0) {
        printf("No shapes added to the picture yet.\n");
        return;
    }
    printf("Current Shapes in the picture:\n");
    for (int i = 0; i < num_shapes; i++) {
        Shape s = shapes[i];
        printf("[%d] ", i + 1);
        switch (s.type) {
            case SHAPE_LINE:
                printf("Line: (%d, %d) to (%d, %d)\n", 
                       s.params.line.x1, s.params.line.y1, 
                       s.params.line.x2, s.params.line.y2);
                break;
            case SHAPE_RECTANGLE:
                printf("Rectangle: corners (%d, %d) and (%d, %d)\n", 
                       s.params.rect.x1, s.params.rect.y1, 
                       s.params.rect.x2, s.params.rect.y2);
                break;
            case SHAPE_CIRCLE:
                printf("Circle: center (%d, %d), radius %d\n", 
                       s.params.circle.xc, s.params.circle.yc, 
                       s.params.circle.r);
                break;
            case SHAPE_TRIANGLE:
                printf("Triangle: vertices (%d, %d), (%d, %d), (%d, %d)\n", 
                       s.params.triangle.x1, s.params.triangle.y1, 
                       s.params.triangle.x2, s.params.triangle.y2, 
                       s.params.triangle.x3, s.params.triangle.y3);
                break;
        }
    }
}

// Menu handler for adding shapes
void add_shape_menu() {
    if (num_shapes >= MAX_SHAPES) {
        printf("Error: Maximum shape capacity reached (%d)!\n", MAX_SHAPES);
        return;
    }

    printf("\n--- Add Object Menu ---\n");
    printf("1. Add Line\n");
    printf("2. Add Rectangle\n");
    printf("3. Add Circle\n");
    printf("4. Add Triangle\n");
    printf("5. Back to Main Menu\n");
    
    int choice = get_int_input("Choose shape to add: ", 1, 5);
    if (choice == 5) return;

    Shape s;
    switch (choice) {
        case 1: // Line
            s.type = SHAPE_LINE;
            printf("Enter details for the Line:\n");
            s.params.line.x1 = get_int_input("Start X (0 to 49): ", 0, WIDTH - 1);
            s.params.line.y1 = get_int_input("Start Y (0 to 19): ", 0, HEIGHT - 1);
            s.params.line.x2 = get_int_input("End X (0 to 49): ", 0, WIDTH - 1);
            s.params.line.y2 = get_int_input("End Y (0 to 19): ", 0, HEIGHT - 1);
            break;
        case 2: // Rectangle
            s.type = SHAPE_RECTANGLE;
            printf("Enter details for the Rectangle (two opposite corner vertices):\n");
            s.params.rect.x1 = get_int_input("Corner 1 X (0 to 49): ", 0, WIDTH - 1);
            s.params.rect.y1 = get_int_input("Corner 1 Y (0 to 19): ", 0, HEIGHT - 1);
            s.params.rect.x2 = get_int_input("Corner 2 X (0 to 49): ", 0, WIDTH - 1);
            s.params.rect.y2 = get_int_input("Corner 2 Y (0 to 19): ", 0, HEIGHT - 1);
            break;
        case 3: // Circle
            s.type = SHAPE_CIRCLE;
            printf("Enter details for the Circle:\n");
            s.params.circle.xc = get_int_input("Center X (0 to 49): ", 0, WIDTH - 1);
            s.params.circle.yc = get_int_input("Center Y (0 to 19): ", 0, HEIGHT - 1);
            s.params.circle.r  = get_int_input("Radius (1 to 25): ", 1, 25);
            break;
        case 4: // Triangle
            s.type = SHAPE_TRIANGLE;
            printf("Enter details for the Triangle (3 vertices):\n");
            s.params.triangle.x1 = get_int_input("Vertex 1 X (0 to 49): ", 0, WIDTH - 1);
            s.params.triangle.y1 = get_int_input("Vertex 1 Y (0 to 19): ", 0, HEIGHT - 1);
            s.params.triangle.x2 = get_int_input("Vertex 2 X (0 to 49): ", 0, WIDTH - 1);
            s.params.triangle.y2 = get_int_input("Vertex 2 Y (0 to 19): ", 0, HEIGHT - 1);
            s.params.triangle.x3 = get_int_input("Vertex 3 X (0 to 49): ", 0, WIDTH - 1);
            s.params.triangle.y3 = get_int_input("Vertex 3 Y (0 to 19): ", 0, HEIGHT - 1);
            break;
    }

    shapes[num_shapes++] = s;
    printf("Shape added successfully!\n");
    render_canvas();
}

// Menu handler for deleting shapes
void delete_shape_menu() {
    if (num_shapes == 0) {
        printf("No shapes to delete.\n");
        return;
    }

    list_shapes();
    int index = get_int_input("Enter the index of the shape to delete (or 0 to cancel): ", 0, num_shapes);
    if (index == 0) return;

    // Shift shapes to fill the gap
    for (int i = index - 1; i < num_shapes - 1; i++) {
        shapes[i] = shapes[i + 1];
    }
    num_shapes--;
    printf("Shape deleted successfully!\n");
    render_canvas();
}

int main() {
    // Initial render of empty canvas
    render_canvas();

    printf("=========================================\n");
    printf("       2D Graphics Editor (Console)      \n");
    printf("=========================================\n");

    while (1) {
        printf("\n--- Main Menu ---\n");
        printf("1. Add Object\n");
        printf("2. Delete Object\n");
        printf("3. Display Picture\n");
        printf("4. List Active Objects\n");
        printf("5. Clear All Objects\n");
        printf("6. Exit\n");

        int choice = get_int_input("Enter your choice: ", 1, 6);

        switch (choice) {
            case 1:
                add_shape_menu();
                break;
            case 2:
                delete_shape_menu();
                break;
            case 3:
                display_canvas_with_borders();
                break;
            case 4:
                list_shapes();
                break;
            case 5:
                num_shapes = 0;
                render_canvas();
                printf("All objects cleared successfully.\n");
                break;
            case 6:
                printf("Exiting 2D Graphics Editor. Goodbye!\n");
                return 0;
        }
    }
}
