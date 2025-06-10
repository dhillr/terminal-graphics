#include <math.h>
#include <graphics.h>

int focal_length = 24;

vec2 rotateX(float y, float z, float theta) {
    return create_vec2((y*cos(theta)-z*sin(theta)),(z*cos(theta)+y*sin(theta)));
}

vec2 rotateY(float x, float z, float theta) {
    return create_vec2((x*cos(theta)-z*sin(theta)), (z*cos(theta)+x*sin(theta)));
}

vec2 rotateZ(float x, float y, float theta) {
    return create_vec2((x*cos(theta)-y*sin(theta)), (y*cos(theta)+x*sin(theta)));
}

vec2 project(vec3 pos) {
    if (pos.z <= 0.1f) pos.z = 0.1f;
    return create_vec2(pos.x * (focal_length / pos.z), pos.y * (focal_length / pos.z));
}

vec2 projectRotate(vec3 pos, float pitch, float yaw, float roll) {
    float rotatedX, rotatedY, rotatedZ;
    vec2 rotatedYaw = rotateY(pos.x, pos.z, yaw);
    rotatedX = rotatedYaw.x;
    rotatedZ = rotatedYaw.y;

    vec2 rotatedPitch = rotateX(pos.y, rotatedZ, yaw);
    rotatedY = rotatedPitch.x;
    rotatedZ = rotatedPitch.y;

    return project(create_vec3(rotatedX, rotatedY, rotatedZ + 30));
}

int main() {
    Display display = create_display(80, 50);

    vec3 points[8] = {
        create_vec3(-10, -10, 10),
        create_vec3(10, -10, 10),
        create_vec3(10, 10, 10),
        create_vec3(-10, 10, 10),
        create_vec3(-10, -10, -10),
        create_vec3(10, -10, -10),
        create_vec3(10, 10, -10),
        create_vec3(-10, 10, -10)
    };

    size_t indices[36] = {
        0, 1, 2,  0, 2, 3,
        4, 5, 6,  4, 6, 7,
        0, 3, 7,  0, 7, 4,
        1, 5, 6,  1, 6, 2,
        3, 2, 6,  3, 6, 7,
        0, 1, 5,  0, 5, 4
    };

    for (int frames = 0; ; frames++) {
        for (int j = 0; j < display.height; j++) {
            for (int i = 0; i < display.width; i++) {
                set_pixel(display, i, j, pixel(0, 0, 0));
            }
        }

        for (int i = 0; i < 36; i += 3) {
            vec2 p[3] = {
                projectRotate(points[indices[i]], 0.04*frames, 0.04*frames, 0),
                projectRotate(points[indices[i+1]], 0.04*frames, 0.04*frames, 0),
                projectRotate(points[indices[i+2]], 0.04*frames, 0.04*frames, 0)
            };
            
            set_triangle(display,
                (int)(p[0].x+display.width/2), (int)(p[0].y+display.height/2),
                (int)(p[1].x+display.width/2), (int)(p[1].y+display.height/2),
                (int)(p[2].x+display.width/2), (int)(p[2].y+display.height/2),
                pixel(255, 255, 255));
        }

        render(display);
        tick(20);
    }

    return terminate(&display);
}