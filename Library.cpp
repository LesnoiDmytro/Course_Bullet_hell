#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;
using namespace std;

int screen_width, screen_heigth;

void set_screen_size(int width, int heigth) {
    screen_width = width;
    screen_heigth = heigth;
}

class Entity {
protected:
    int x, y, r;
public:
    Entity(int n_x, int n_y) {
        x = n_x;
        y = n_y;
    }

    vector<int> get_position() { return vector<int>{x - r, y - r}; }
};

class Player : public Entity {
    int move;
public:
    Player(int n_r) : Entity(screen_width / 2, screen_heigth / 2) { r = n_r / 2; move = screen_width / 120; }

    void set_to_start() {
        x = screen_width/ 2;
        y = screen_heigth / 2;
    }

    void Move(int to_x, int to_y) {
        double a = 1;
        if (to_x && to_y)
            a = 0.707;
        x += move * to_x * a;
        y += -move * to_y * a;
        if (x - r < 0)
            x = r;
        else if (x + r > screen_width)
            x = screen_width - r;
        if (y - r < 0)
            y = r;
        else if (y + r > screen_heigth)
            y = screen_heigth - r;
    }

    int get_x() { return x; }
    int get_y() { return y; }
    int get_r() { return r; }
};

class Enemy : public Entity {
protected:
    int frames, n;
public:
    Enemy(int n_x, int n_y) : Entity(n_x, n_y) { frames = 0; }
};

class Ray : public Enemy {
protected:
    vector<vector<int>> rays;
    py::tuple rays_color;

    bool collision(Player player) {
        int A, B, C, distance;
        for (int i = 0; i < n; i++) {
            A = rays[i][3] - rays[i][1];
            B = rays[i][0] - rays[i][2];
            C = -(A * rays[i][0] + B * rays[i][1]);
            distance = abs((A * player.get_x() + B * player.get_y() + C) / sqrt(A * A + B * B));
            if (distance <= player.get_r())
                return true;
        }
        return false;
    }

public:
    Ray(Player player, int n_r) : Enemy(0, 0) { 
        r = n_r;
        rays_color = py::make_tuple(0, 0, 0);
    }

    int get_r() { return r; }
    vector<vector<int>> to_draw_rays() {
        vector<vector<int>> draw_info;
        draw_info.push_back(vector<int> {n + 1});
        for (int i = 0; i < n; i++) {
            draw_info.push_back(vector<int> {rays[i][0], rays[i][1]});
            draw_info.push_back(vector<int> {rays[i][2], rays[i][3]});
        }
        return draw_info;
    }
    py::tuple get_rays_color() { return rays_color; }
};

class HorizontalRay : public Ray {
public:
    HorizontalRay(Player player, int n_r, int rand) : Ray(player, n_r) { 
        rays.push_back(vector<int> {player.get_x(), 0, player.get_x(), screen_heigth}); 
        if (rand % 2) {
            y = screen_heigth - r;
            rays[0][3] -= 2*r;
        }
        else {
            y = r;
            rays[0][1] += 2*r;
        }
        x = player.get_x();
        n = 1;
    }

    bool action(Player player) {
        if (frames < 90) {
            rays[0][0] = rays[0][2] = x = player.get_x();
            
        }
        else if (frames < 120) {
            n = 0;
        }
        else {
            n = 1;
            rays_color = py::make_tuple(255, 0, 0);
            return collision(player);
        }
        frames += 1;
        return false;
    }
};

class VerticalRay : public Ray {
public:
    VerticalRay(Player player, int n_r, int rand) : Ray(player, n_r) {
        rays.push_back(vector<int> {0, player.get_y(), screen_width, player.get_y()}); 
        if (rand % 2) {
            x = screen_width - r;
            rays[0][2] -= 2*r;
        }
        else {
            x = r;
            rays[0][0] += 2*r;
        }
        y = player.get_y();
        n = 1; 
    }

    bool action(Player player) {
        if (frames < 90) {
            rays[0][1] = rays[0][3] = y = player.get_y();
        }
        else if (frames < 120) {
            n = 0;
        }
        else {
            n = 1;
            rays_color = py::make_tuple(255, 0, 0);
            return collision(player);
        }
        frames += 1;
        return false;
    }
};

class DiagonalRay : public Ray{
    int n_save;
public:
    DiagonalRay(Player player, int n_r, int rand) : Ray(player, n_r) {
        n = n_save = 0;
        int x_r, y_r;
        int dir_x = 1, dir_y = 1;
        if (rand % 2) {
            x = screen_width - r;
            x_r = screen_width - 2 * r;
            dir_x = -1;
        }
        else {
            x = r;
            x_r = 2 * r;
        }
        if (rand > 1) {
            y = screen_heigth - r;
            y_r = screen_heigth - 2 * r;
            dir_y = -1;
        }
        else {
            y = r;
            y_r = 2 * r;
        }
        while (x_r > 0 && x_r < screen_width) {
            int x_old = x_r, y_old = y_r;
            int add = screen_heigth;
            if (y_r != 0 && y_r != screen_heigth)
                add = screen_heigth - 2 * r;
            x_r += dir_x * add;
            if (x_r > screen_width) {
                x_r = screen_width;
                y_r += dir_y * (screen_width - x_old);
            }
            else if (x_r < 0) {
                x_r = 0;
                y_r += dir_y * x_old;
            }
            else {
                y_r += dir_y * add;
                dir_y *= -1;
            }
            rays.push_back(vector<int> { x_old, y_old, x_r, y_r });
            n_save = ++n;
        }
    }

    bool action(Player player) {
        if (frames == 90) {
            n = 0;
        }
        else if (frames > 119){
            n = n_save;
            rays_color = py::make_tuple(255, 0, 0);
            return collision(player);
        }
        frames += 1;
        return false;
    }
};

class ChainBalls : public Enemy {
protected:
    int chain_length;
    vector<int> chain;
    vector<int> balls;

    bool collision(Player player) {
        double a, b, c;
        double t, dist;
        int dx, dy;
        a = chain[3] - chain[1];
        b = chain[0] - chain[2];
        c = chain[2] * chain[1] - chain[0] * chain[3];
        t = -(a * player.get_x() + b * player.get_y() + c) / (a * a + b * b);
        dx = player.get_x() + a * t;
        dy = player.get_y() + b * t;
        if (between_segment(dx, dy)) {
            dist = sqrt((a * t) * (a * t) + (b * t) * (b * t));
            if (dist <= player.get_r())
                return true;
        }
        for (int i = 0; i < n; i++) {
            dist = sqrt((balls[i * 2] - player.get_x()) * (balls[i * 2] - player.get_x()) + (balls[i * 2 + 1] - player.get_y()) * (balls[i * 2 + 1] - player.get_y()));
            if (dist <= r + player.get_r())
                return true;
        }
        return false;
    }

public:
    ChainBalls(int chain_len, int n_r) :
        Enemy(rand() % (screen_width - 2 * n_r - 2 * chain_len) + n_r + chain_len, rand() % (screen_heigth - 2 * n_r - 2 * chain_len) + n_r + chain_len) {
        chain_length = chain_len;
        r = n_r;
        chain = vector<int>{ x, y, x, y };
    }

    bool between_segment(int dx, int dy) {
        int x1 = chain[0], x2 = chain[2], y1 = chain[1], y2 = chain[3], temp;
        if (x1 > x2) {
            temp = x1;
            x1 = x2;
            x2 = temp;
        }
        if (y1 > y2) {
            temp = y1;
            y1 = y2;
            y2 = temp;
        }
        return dx >= x1 && dx <= x2 && dy >= y1 && dy <= y2;
    }
    int get_n() { return n; }
    vector<int> get_balls() { 
        vector<int> to_ret; 
        for (int i = 0; i < n * 2; i++)
            to_ret.push_back(balls[i] - r);
        return to_ret;
    }
    vector<int> get_chain() { return chain; }
};

class OneBall : public ChainBalls {
public:
    OneBall(int chain_len, int n_r) : ChainBalls(chain_len, n_r) {
        balls = vector<int>{ x + chain_length, y };
    }

    bool action(Player player) {
        if (frames < 60) {
            n = 0;
            chain[2] += chain_length / 60;
        }
        else {
            n = 1;
            float angle = 0.0524;
            int temp;
            temp = (balls[0] - x) * cos(angle) - (balls[1] - y) * sin(angle) + x;
            balls[1] = (balls[0] - x) * sin(angle) + (balls[1] - y) * cos(angle) + y;
            balls[0] = temp;
            chain[2] = balls[0];
            chain[3] = balls[1];
            return collision(player);
        }
        frames++;
        return false;
    }
};

class TwoBalls : public ChainBalls {
public:
    TwoBalls(int chain_len, int n_r) : ChainBalls(chain_len, n_r) {
        balls = vector<int>{ x + chain_length, y, x - chain_length, y };
    }

    bool action(Player player) {
        if (frames < 60) {
            n = 0;
            chain[2] += chain_length / 60;
            chain[0] -= chain_length / 60;
        }
        else {
            n = 2;
            float angle = 0.0524;
            int temp;
            temp = (balls[0] - x) * cos(angle) - (balls[1] - y) * sin(angle) + x;
            balls[1] = (balls[0] - x) * sin(angle) + (balls[1] - y) * cos(angle) + y;
            balls[0] = temp;
            temp = (balls[2] - x) * cos(angle) - (balls[3] - y) * sin(angle) + x;
            balls[3] = (balls[2] - x) * sin(angle) + (balls[3] - y) * cos(angle) + y;
            balls[2] = temp;
            chain[0] = balls[0];
            chain[1] = balls[1];
            chain[2] = balls[2];
            chain[3] = balls[3];
            return collision(player);
        }
        frames++;
        return false;
    }
};

class Cannon : public Enemy {
protected:
    vector<int> bullets;
    vector<int> diraction;
    int b_r, speed;

    bool collision(Player player) {
        double dist;
        for (int i = 0; i < n; i++) {
            dist = sqrt((bullets[i * 2] - player.get_x()) * (bullets[i * 2] - player.get_x()) + (bullets[i * 2 + 1] - player.get_y()) * (bullets[i * 2 + 1] - player.get_y()));
            if (dist < b_r + player.get_r())
                return true;
        }
        return false;
    }
public:
    Cannon(int random, int n_r, int n_b, int n_speed) : Enemy(0, 0) {
        r = n_r;
        b_r = n_b;
        speed = n_speed;
        if (random < 2) {
            y = rand() % (screen_heigth - 4 * r) + 2 * r;
            if (random % 2)
                x = screen_width - r;
            else
                x = r;
        }
        else {
            x = rand() % (screen_width - 4 * r) + 2 * r;
            if (random % 2) {
                y = screen_heigth - r;
            }
            else {
                y = r;
            }
        }
    }

    int get_n() { return n; }
    vector<int> get_bullets() {
        vector<int> to_ret;
        for (int i = 0; i < n * 2; i++)
            to_ret.push_back(bullets[i] - b_r);
        return to_ret;
    }
};

class SimpleCannon : public Cannon {
public:
    SimpleCannon(int random, int n_r, int n_b, int n_speed) : Cannon(random, n_r, n_b, n_speed) {
        n = 1;
        bullets = vector<int>{ x, y };
        switch (random) {
        case 0:
            diraction = vector<int> { 1, 0 };
            break;
        case 1:
            diraction = vector<int>{ -1, 0 };
            break;
        case 2:
            diraction = vector<int>{ 0, 1 };
            break;
        case 3:
            diraction = vector<int>{ 0, -1 };
            break;
        }
    }

    bool action(Player player) {
        if (frames < 30) {
            frames++;
            return false;
        }
        if (diraction[1])
            bullets[0] += speed * diraction[0];
        else
            bullets[0] += speed * diraction[0] * 1.41;
        if (diraction[0])
            bullets[1] += speed * diraction[1];
        else 
            bullets[1] += speed * diraction[1] * 1.41;
        if (bullets[0] <= b_r || bullets[0] >= screen_width - b_r) {
            if (bullets[0] <= b_r)
                bullets[0] = b_r;
            else
                bullets[0] = screen_width - b_r;
            if (!diraction[1])
                diraction[1] = -1 + rand() % 2 * 2;
            diraction[0] *= -1;
        }
        if (bullets[1] <= b_r || bullets[1] >= screen_heigth - b_r) {
            if (bullets[1] <= b_r)
                bullets[1] = b_r;
            else
                bullets[1] = screen_heigth - b_r;
            if (!diraction[0])
                diraction[0] = -1 + rand() % 2 * 2;
            diraction[1] *= -1;
        }
        return collision(player);
    }
};

class TripleCannon : public Cannon {
public:
    TripleCannon(int random, int n_r, int n_b, int n_speed) : Cannon(random, n_r, n_b, n_speed) {
        n = 3;
        bullets = vector<int>{ x, y, x, y, x, y };
        switch (random) {
        case 0:
            diraction = vector<int>{ -1, -1, -1, 0, -1, 1 };
            break;
        case 1:
            diraction = vector<int>{ 1, -1, 1, 0, 1, 1 };
            break;
        case 2:
            diraction = vector<int>{ -1, -1, 0, -1, 1, -1 };
            break;
        case 3:
            diraction = vector<int>{ -1, 1, 0, 1, 1, 1 };
            break;
        }
    }

    bool action(Player player) {
        if (frames < 30) {
            frames++;
            return false;
        }
        bool result = false;
        for (int i = 0; i < n; i++) {
            if (diraction[i * 2 + 1])
                bullets[i * 2] += speed * diraction[i * 2];
            else 
                bullets[i * 2] += speed * diraction[i * 2] * 1.41;
            if (diraction[i * 2])
                bullets[i * 2 + 1] += speed * diraction[i * 2 + 1];
            else
                bullets[i * 2 + 1] += speed * diraction[i * 2 + 1] * 1.41;
            if (bullets[i * 2] <= b_r) {
                bullets[i * 2] = b_r;
                diraction[i * 2] *= -1;
            }
            if (bullets[i * 2] >= screen_width - b_r) {
                bullets[i * 2] = screen_width - b_r;
                diraction[i * 2] *= -1;
            }
            if (bullets[i * 2 + 1] <= b_r) {
                bullets[i * 2 + 1] = b_r;
                diraction[i * 2 + 1] *= -1;
            }
            if (bullets[i * 2 + 1] >= screen_heigth - b_r) {
                bullets[i * 2 + 1] = screen_heigth - b_r;
                diraction[i * 2 + 1] *= -1;
            }
            if (collision(player))
                result = true;
        }
        return result;
    }
};

PYBIND11_MODULE(library, m) {
    m.def("set_screen_size", &set_screen_size);

    py::class_<Entity>(m, "Entity")
        .def("get_position", &Entity::get_position);

    py::class_<Player, Entity>(m, "Player")
        .def(py::init<int>())
        .def("Move", &Player::Move)
        .def("set_to_start", &Player::set_to_start);

    py::class_<Ray, Entity>(m, "Ray")
        .def("get_r", &Ray::get_r)
        .def("to_draw_rays", &Ray::to_draw_rays)
        .def("rays_color", &Ray::get_rays_color);

    py::class_<HorizontalRay, Ray>(m, "HorizontalRay")
        .def(py::init<Player, int, int>())
        .def("action", &HorizontalRay::action);

    py::class_<VerticalRay, Ray>(m, "VerticalRay")
        .def(py::init<Player, int, int>())
        .def("action", &VerticalRay::action);

    py::class_<DiagonalRay, Ray>(m, "DiagonalRay")
        .def(py::init<Player, int, int>())
        .def("action", &DiagonalRay::action);

    py::class_<ChainBalls, Entity>(m, "ChainBalls")
        .def("get_n", &ChainBalls::get_n)
        .def("get_balls", &ChainBalls::get_balls)
        .def("get_chain", &ChainBalls::get_chain);

    py::class_<OneBall, ChainBalls>(m, "OneBall")
        .def(py::init<int, int>())
        .def("action", &OneBall::action);

    py::class_<TwoBalls, ChainBalls>(m, "TwoBalls")
        .def(py::init<int, int>())
        .def("action", &TwoBalls::action);

    py::class_<Cannon, Entity>(m, "Cannon")
        .def("get_n", &Cannon::get_n)
        .def("get_bullets", &Cannon::get_bullets);

    py::class_<SimpleCannon, Cannon>(m, "SimpleCannon")
        .def(py::init<int, int, int, int>())
        .def("action", &SimpleCannon::action);

    py::class_<TripleCannon, Cannon>(m, "TripleCannon")
        .def(py::init<int, int, int, int>())
        .def("action", &TripleCannon::action);
}