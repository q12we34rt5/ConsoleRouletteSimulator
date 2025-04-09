#include "lib/ArgCLITool/ArgParser.hpp"
#include "lib/CMap/cmap.h"
#include "lib/PixelMatrix/ConsoleColor.h"
#include "lib/PixelMatrix/PixelMatrix.h"
#include "lib/Q3Engine/Buffer.hpp"
#include "lib/Q3Engine/Math.hpp"
#include "lib/Q3Engine/Rasterizer.hpp"
#include "lib/Q3Engine/Shader.hpp"
#include "lib/Q3Engine/Texture.hpp"
#include "lib/Q3Engine/Utils.hpp"
#include <atomic>
#include <cmath>
#include <iostream>
#include <mutex>
#include <random>
#include <sstream>
#include <thread>

struct Config {
    int n_numbers;
    float angle;
    float radius;
    int size;
    int rounds;
    int steps;
    q3::RGBColor text_color;
    q3::RGBColor highlight_color;
    q3::Rasterizer::AA_MODE aa_mode;
    int max_fps;
    int max_tps;
    bool show_metrics;
    bool precise_timing;
} config;

// q3::Texture numbers[] = {
//     q3::Texture(q3::loadBmpTexture("assets/number_0.bmp", {255, 255, 255})),
//     q3::Texture(q3::loadBmpTexture("assets/number_1.bmp", {255, 255, 255})),
//     q3::Texture(q3::loadBmpTexture("assets/number_2.bmp", {255, 255, 255})),
//     q3::Texture(q3::loadBmpTexture("assets/number_3.bmp", {255, 255, 255})),
//     q3::Texture(q3::loadBmpTexture("assets/number_4.bmp", {255, 255, 255})),
//     q3::Texture(q3::loadBmpTexture("assets/number_5.bmp", {255, 255, 255})),
//     q3::Texture(q3::loadBmpTexture("assets/number_6.bmp", {255, 255, 255})),
//     q3::Texture(q3::loadBmpTexture("assets/number_7.bmp", {255, 255, 255})),
//     q3::Texture(q3::loadBmpTexture("assets/number_8.bmp", {255, 255, 255})),
//     q3::Texture(q3::loadBmpTexture("assets/number_9.bmp", {255, 255, 255}))};

constexpr int TEXTURE_WIDTH = 200;
constexpr int TEXTURE_HEIGHT = 400;

q3::Texture numbers[] = {
    q3::Texture(std::make_shared<q3::GraphicsBuffer<q3::RGBColor>>(std::vector<q3::RGBColor>{
        #include "assets/number_0.data"
    }, TEXTURE_WIDTH, TEXTURE_HEIGHT)),
    q3::Texture(std::make_shared<q3::GraphicsBuffer<q3::RGBColor>>(std::vector<q3::RGBColor>{
        #include "assets/number_1.data"
    }, TEXTURE_WIDTH, TEXTURE_HEIGHT)),
    q3::Texture(std::make_shared<q3::GraphicsBuffer<q3::RGBColor>>(std::vector<q3::RGBColor>{
        #include "assets/number_2.data"
    }, TEXTURE_WIDTH, TEXTURE_HEIGHT)),
    q3::Texture(std::make_shared<q3::GraphicsBuffer<q3::RGBColor>>(std::vector<q3::RGBColor>{
        #include "assets/number_3.data"
    }, TEXTURE_WIDTH, TEXTURE_HEIGHT)),
    q3::Texture(std::make_shared<q3::GraphicsBuffer<q3::RGBColor>>(std::vector<q3::RGBColor>{
        #include "assets/number_4.data"
    }, TEXTURE_WIDTH, TEXTURE_HEIGHT)),
    q3::Texture(std::make_shared<q3::GraphicsBuffer<q3::RGBColor>>(std::vector<q3::RGBColor>{
        #include "assets/number_5.data"
    }, TEXTURE_WIDTH, TEXTURE_HEIGHT)),
    q3::Texture(std::make_shared<q3::GraphicsBuffer<q3::RGBColor>>(std::vector<q3::RGBColor>{
        #include "assets/number_6.data"
    }, TEXTURE_WIDTH, TEXTURE_HEIGHT)),
    q3::Texture(std::make_shared<q3::GraphicsBuffer<q3::RGBColor>>(std::vector<q3::RGBColor>{
        #include "assets/number_7.data"
    }, TEXTURE_WIDTH, TEXTURE_HEIGHT)),
    q3::Texture(std::make_shared<q3::GraphicsBuffer<q3::RGBColor>>(std::vector<q3::RGBColor>{
        #include "assets/number_8.data"
    }, TEXTURE_WIDTH, TEXTURE_HEIGHT)),
    q3::Texture(std::make_shared<q3::GraphicsBuffer<q3::RGBColor>>(std::vector<q3::RGBColor>{
        #include "assets/number_9.data"
    }, TEXTURE_WIDTH, TEXTURE_HEIGHT))};

class SolidShader : public q3::Shader {
public:
    std::size_t getContextSize() const override { return 0; }
    bool vertexShader(q3::Vertex& v0, q3::Vertex& v1, q3::Vertex& v2, void* data0, void* data1, void* data2, void* context) override
    {
        v0 = transform.dot(v0);
        v1 = transform.dot(v1);
        v2 = transform.dot(v2);
        return true;
    }
    q3::RGBColor fragmentShader(const q3::Triangle& triangle, const q3::Barycentric& barycentric, void* data0, void* data1, void* data2, const void* context) override
    {
        return color;
    }

public:
    q3::Matrix4 transform;
    q3::RGBColor color = {0, 0, 0};
};

class TextShader : public q3::Shader {
public:
    std::size_t getContextSize() const override { return 0; }
    bool vertexShader(q3::Vertex& v0, q3::Vertex& v1, q3::Vertex& v2, void* data0, void* data1, void* data2, void* context) override
    {
        v0 = transform.dot(v0);
        v1 = transform.dot(v1);
        v2 = transform.dot(v2);
        return true;
    }
    q3::RGBColor fragmentShader(const q3::Triangle& triangle, const q3::Barycentric& barycentric, void* data0, void* data1, void* data2, const void* context) override
    {
        struct VertexData {
            q3::Vector2& uv;
        };

        auto v0 = reinterpret_cast<VertexData*>(data0);
        auto v1 = reinterpret_cast<VertexData*>(data1);
        auto v2 = reinterpret_cast<VertexData*>(data2);
        auto uv = q3::Shader::perspectiveCorrectInterpolate(v0->uv, v1->uv, v2->uv, triangle, barycentric);

        // Only draw pixel if alpha in texture is non-zero (avoid rendering background)
        return texture.sample(uv).a != 0 ? color : q3::RGBColor{0, 0, 0, 0};
    }

public:
    q3::Matrix4 transform;
    q3::Texture texture;
    q3::RGBColor color;
};

class Object {
public:
    Object()
        : vertices(std::make_shared<q3::DataBuffer<q3::Vector3>>()),
          indices(std::make_shared<q3::DataBuffer<uint32_t>>()),
          uvs(std::make_shared<q3::DataBuffer<q3::Vector2>>())
    {
        updateMatrix();
    }

    void rotateBufferData(float angle)
    {
        for (auto& vertex : *vertices) {
            float x = vertex.x;
            float y = vertex.y;
            vertex.x = x * std::cos(angle) - y * std::sin(angle);
            vertex.y = x * std::sin(angle) + y * std::cos(angle);
        }
    }

    void scaleBufferData(float sx, float sy, float sz = 1.0f)
    {
        for (auto& vertex : *vertices) {
            vertex.x *= sx;
            vertex.y *= sy;
            vertex.z *= sz;
        }
    }

    void translateBufferData(float tx, float ty, float tz = 0.0f)
    {
        for (auto& vertex : *vertices) {
            vertex.x += tx;
            vertex.y += ty;
            vertex.z += tz;
        }
    }

    void setRotation(float angle)
    {
        rotation = angle;
        updateMatrix();
    }

    void setScale(float sx, float sy, float sz = 1.0f)
    {
        scale = {sx, sy, sz};
        updateMatrix();
    }

    void setTranslation(float tx, float ty, float tz = 0.0f)
    {
        translation = {tx, ty, tz};
        updateMatrix();
    }

    void setColor(q3::RGBColor color) { this->color = std::move(color); }
    void setVertices(std::shared_ptr<q3::DataBuffer<q3::Vector3>> vertices) { this->vertices = vertices; }
    void setIndices(std::shared_ptr<q3::DataBuffer<uint32_t>> indices) { this->indices = indices; }
    void setUVs(std::shared_ptr<q3::DataBuffer<q3::Vector2>> uvs) { this->uvs = uvs; }

    const q3::Vector3& getScale() const { return scale; }
    const q3::Vector3& getTranslation() const { return translation; }
    float getRotation() const { return rotation; }
    const q3::Matrix4& getTransformMatrix() const { return transform_matrix; }
    const q3::RGBColor& getColor() const { return color; }
    const std::shared_ptr<q3::DataBuffer<q3::Vector3>>& getVertices() const { return vertices; }
    const std::shared_ptr<q3::DataBuffer<uint32_t>>& getIndices() const { return indices; }
    const std::shared_ptr<q3::DataBuffer<q3::Vector2>>& getUVs() const { return uvs; }

protected:
    void updateMatrix()
    {
        q3::Matrix4 scale_m = q3::createScaleMatrix(scale);
        q3::Matrix4 rotate_m = q3::createRotationMatrix(rotation, {0.0f, 0.0f, -1.0f});
        q3::Matrix4 translate_m = q3::createTranslationMatrix(translation);

        // Apply scale -> then rotate -> then translate (SRT order)
        transform_matrix = translate_m.dot(rotate_m).dot(scale_m);
    }

protected:
    q3::Vector3 scale = {1.0f, 1.0f, 1.0f};
    q3::Vector3 translation = {0.0f, 0.0f, 0.0f};
    float rotation = 0.0f;
    q3::Matrix4 transform_matrix;
    q3::RGBColor color = {0, 0, 0};
    std::shared_ptr<q3::DataBuffer<q3::Vector3>> vertices;
    std::shared_ptr<q3::DataBuffer<uint32_t>> indices;
    std::shared_ptr<q3::DataBuffer<q3::Vector2>> uvs;
};

class Fan : public Object {
public:
    Fan(float radius, float angle, int n_triangles = 50)
        : radius(radius), angle(angle), n_triangles(n_triangles)
    {
        generateVertices();
    }

private:
    void generateVertices()
    {
        float start_angle = -angle / 2;
        float end_angle = angle / 2;
        float step = (end_angle - start_angle) / n_triangles;

        // Create a triangle fan geometry centered at (0,0) spanning 'angle'
        // Used to represent a single slice of the roulette
        vertices->push_back({0.0f, 0.0f, 0.0f});
        vertices->push_back({radius * std::cos(start_angle), radius * std::sin(start_angle), 0.0f});

        for (int i = 0; i < n_triangles; ++i) {
            float angle1 = start_angle + (i + 1) * step;
            vertices->push_back({radius * std::cos(angle1), radius * std::sin(angle1), 0.0f});

            indices->push_back(0);
            indices->push_back(i + 1);
            indices->push_back(i + 2);
        }
    }

private:
    float radius;
    float angle;
    int n_triangles;
};

class TextBox : public Object {
public:
    TextBox(q3::Texture text) : text(text)
    {
        updateDimensions();
        setWidth(0.3f, true);
        translateBufferData(-width / 2, 0.3f, -0.05f);
        rotateBufferData(-M_PI / 2);
    }

    void setText(q3::Texture text)
    {
        this->text = text;
        updateDimensions();
    }

    void setWidth(float width, bool keep_aspect_ratio = true)
    {
        if (keep_aspect_ratio) {
            float aspect_ratio = this->width / this->height;
            this->height = width / aspect_ratio;
        }
        this->width = width;
        updateBufferData();
    }

    void setHeight(float height, bool keep_aspect_ratio = true)
    {
        if (keep_aspect_ratio) {
            float aspect_ratio = this->width / this->height;
            this->width = height * aspect_ratio;
        }
        this->height = height;
        updateBufferData();
    }

    const q3::Texture& getText() const { return text; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }

private:
    void updateBufferData()
    {
        generateBufferData();
        updateMatrix();
    }

    void updateDimensions()
    {
        this->width = text.getImageBuffer()->getWidth();
        this->height = text.getImageBuffer()->getHeight();
        updateBufferData();
    }

    void generateBufferData()
    {
        // Define a textured quad for displaying the number texture
        *vertices = q3::DataBuffer<q3::Vector3>({
            {0.0f, 0.0f, 0.0f},
            {width, 0.0f, 0.0f},
            {width, height, 0.0f},
            {0.0f, height, 0.0f},
        });
        *uvs = q3::DataBuffer<q3::Vector2>({
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f},
        });
        *indices = q3::DataBuffer<uint32_t>({0, 1, 2, 0, 2, 3});
    }

private:
    q3::Texture text;
    float width = 0.0f;
    float height = 0.0f;
};

class Roulette {
public:
    Roulette(size_t n_numbers, float radius, q3::RGBColor text_color, q3::RGBColor highlight_color, int n_triangles = 50)
        : n_numbers(n_numbers), radius(radius), text_color(text_color), highlight_color(highlight_color), n_triangles(n_triangles)
    {
        angle_step = 2 * M_PI / n_numbers;
        generateFanAndTextBox();
        generatePin();
    }

    void setRotation(float angle)
    {
        rotation = angle;
        updateObjects();
    }

    void rotate(float delta_angle)
    {
        rotation = std::fmod(rotation + delta_angle, 2 * M_PI);
        updateObjects();
    }

    void setSize(float size)
    {
        radius = size;
        updateObjects();
    }

    Fan& getFan(int index)
    {
        if (index < 1 || index > n_numbers) {
            throw std::out_of_range("Index out of range");
        }
        return fans[index - 1];
    }

    TextBox& getTextBox(int index)
    {
        if (index < 1 || index > n_numbers) {
            throw std::out_of_range("Index out of range");
        }
        return text_boxes[index - 1];
    }

    int calculatePointedNumber(float rotation) const
    {
        float pointer_angle = M_PI / 2;

        // Adjust angle so that 0 is aligned with the pointer (pointing upwards)
        // Then map the adjusted angle to the corresponding sector index
        float delta_angle = pointer_angle - rotation + angle_step / 2;
        while (delta_angle < 0) { delta_angle += 2 * M_PI; }
        int index = std::fmod(delta_angle, 2 * M_PI) / angle_step;
        return index + 1;
    }

    int getPointedNumber() const
    {
        return calculatePointedNumber(rotation);
    }

    void render(q3::Rasterizer& rasterizer)
    {
        // Draw all the fans and text boxes
        for (size_t i = 0; i < n_numbers; ++i) {
            // Set fan rotation based on index and the current global rotation
            float fan_rotation = rotation + i * angle_step;
            fans[i].setRotation(fan_rotation);
            solid_shader.transform = fans[i].getTransformMatrix();
            solid_shader.color = fans[i].getColor();
            rasterizer.drawBuffer(*fans[i].getVertices(), *fans[i].getIndices(), solid_shader, dummy_sampler);

            // Draw the text box corresponding to the number
            text_boxes[i].setRotation(fan_rotation);
            q3::AutoDataBufferSampler text_box_sampler(text_boxes[i].getUVs());
            texture_shader.texture = text_boxes[i].getText();
            texture_shader.transform = text_boxes[i].getTransformMatrix();
            texture_shader.color = text_boxes[i].getColor();
            rasterizer.drawBuffer(*text_boxes[i].getVertices(), *text_boxes[i].getIndices(), texture_shader, text_box_sampler);
        }

        // Draw the pin
        for (const auto& pin_part : pin) {
            solid_shader.transform = pin_part.getTransformMatrix();
            solid_shader.color = pin_part.getColor();
            rasterizer.drawBuffer(*pin_part.getVertices(), *pin_part.getIndices(), solid_shader, dummy_sampler);
        }
    }

private:
    void generateFanAndTextBox()
    {
        auto cmap = cm::CMap::palettes["accent"].setRange(0, n_numbers);
        for (size_t i = 0; i < n_numbers; ++i) {
            // Create each fan
            Fan fan(radius, angle_step, std::max<int>(n_triangles / n_numbers, 1));
            auto color = cmap[i];
            fan.setColor({color.R, color.G, color.B});
            fans.push_back(std::move(fan));

            // Create each text box (numbers 1-9 in a loop)
            TextBox text_box(numbers[i % 9 + 1]);
            text_box.setColor(text_color);
            text_boxes.push_back(std::move(text_box));
        }
    }

    void generatePin()
    {
        Fan pin_face(0.3f, M_PI / 4, 1);
        pin_face.setColor({255, 0, 0});
        pin_face.setRotation(M_PI / 2);
        pin_face.setTranslation(0.0f, -0.75f, -0.2f);

        Fan pin_shadow(0.3f, M_PI / 4, 1);
        pin_shadow.setColor({0, 0, 0});
        pin_shadow.setRotation(M_PI / 2);
        pin_shadow.setTranslation(0.0f, -0.7f, -0.1f);
        pin_shadow.scaleBufferData(1.2f, 1.2f, 1.0f);

        pin.push_back(std::move(pin_face));
        pin.push_back(std::move(pin_shadow));
    }

    void updateObjects()
    {
        int pointed_number = getPointedNumber();

        // Update each fan's position and text box's position
        for (size_t i = 0; i < n_numbers; ++i) {
            float fan_rotation = rotation + i * angle_step;
            fans[i].setRotation(fan_rotation);
            text_boxes[i].setRotation(fan_rotation);
            text_boxes[i].setColor(i + 1 == pointed_number ? highlight_color : text_color);
        }
    }

private:
    size_t n_numbers;
    float radius;
    q3::RGBColor text_color;
    q3::RGBColor highlight_color;
    int n_triangles;
    float angle_step;
    float rotation = 0.0f;

    // Fans and corresponding text boxes
    std::vector<Fan> fans;
    std::vector<TextBox> text_boxes;

    // Winning number indicator
    std::vector<Fan> pin;

    // Shaders and samplers
    SolidShader solid_shader;
    TextShader texture_shader;
    q3::DummyDataBufferSampler dummy_sampler;
};

class RotationManager {
public:
    RotationManager(float target_angle, int steps)
        : target_angle(target_angle), total_steps(steps + 1), current_angle(0.0f), current_step(0)
    {
        reset();
    }

    void setTargetAngle(float angle) { target_angle = angle; }
    void setTotalSteps(int steps) { total_steps = steps + 1; }

    float getCurrentAngle() const { return current_angle; }
    int getCurrentStep() const { return current_step; }
    int getTotalSteps() const { return total_steps - 1; }

    void reset()
    {
        remaining_angle = config.rounds * 2 * M_PI + (target_angle - current_angle);
    }

    bool step()
    {
        if (remaining_angle <= 0) { return true; }
        int remaining_steps = total_steps - current_step;

        // Apply acceleration-like behavior: larger step at first, gradually slowing down
        float delta_angle = remaining_angle * 2 / remaining_steps;
        current_angle = std::fmod(current_angle + delta_angle, 2 * M_PI);
        remaining_angle -= delta_angle;
        ++current_step;
        return false;
    }

private:
    float target_angle;
    int total_steps;
    int current_step;
    float current_angle;
    float remaining_angle;
};

class Renderer {
public:
    Renderer(int width, int height)
        : pixel_matrix(width, height)
    {
        // Save cursor position
        std::cout << "\033[s";
    }

    void setBuffer(std::shared_ptr<q3::GraphicsBuffer<q3::RGBColor>> buffer)
    {
        std::lock_guard<std::mutex> lock(buffer_mutex);
        framebuffer = buffer;
    }

    void render()
    {
        {
            std::lock_guard<std::mutex> lock(buffer_mutex);

            // Copy the contents of framebuffer into the internal pixel_matrix
            // This step must be locked to avoid reading from a buffer that is being changed
            if (framebuffer) {
                graphicsBufferToPixelMatrix(*framebuffer, pixel_matrix);
            }
        }

        // Restore saved cursor position to overwrite previous frame
        std::cout << "\033[u";

        // Render the pixel matrix to console
        std::cout << pixel_matrix << std::flush;
    }

private:
    static void graphicsBufferToPixelMatrix(const q3::GraphicsBuffer<q3::RGBColor>& buffer, PixelMatrix& pixel_matrix)
    {
        for (uint32_t y = 0; y < buffer.getHeight(); ++y) {
            for (uint32_t x = 0; x < buffer.getWidth(); ++x) {
                auto color = buffer.getValue(x, y);
                pixel_matrix[y][x].color = {color.r, color.g, color.b};
                if (color.a == 0) {
                    pixel_matrix.disable(y, x);
                } else {
                    pixel_matrix.enable(y, x);
                }
            }
        }
    }

private:
    PixelMatrix pixel_matrix;

    // Framebuffer to be rendered (shared from logic thread)
    std::shared_ptr<q3::GraphicsBuffer<q3::RGBColor>> framebuffer;

    // Mutex to protect framebuffer access between render and setBuffer
    std::mutex buffer_mutex;
};

class RateTimer {
public:
    RateTimer(double target_rate_hz, bool high_precision = true)
        : high_precision_mode(high_precision)
    {
        if (target_rate_hz <= 0.0 || target_rate_hz == std::numeric_limits<double>::infinity()) {
            uncapped = true;
        } else {
            target_duration = std::chrono::duration<double>(1.0 / target_rate_hz);
        }
        last_time = std::chrono::steady_clock::now();
    }

    void waitNext()
    {
        auto now = std::chrono::steady_clock::now();

        if (!uncapped) {
            auto elapsed = now - last_time;

            if (high_precision_mode) {
                // Subtract 1ms to avoid overshooting
                auto wait_time = target_duration - elapsed - std::chrono::milliseconds(1);
                if (wait_time > std::chrono::milliseconds(0)) {
                    std::this_thread::sleep_for(wait_time);
                }
                // Busy wait
                while (std::chrono::steady_clock::now() - last_time < target_duration);
            } else {
                // Simple sleep-only version
                if (elapsed < target_duration) {
                    std::this_thread::sleep_for(target_duration - elapsed);
                }
            }
        }

        auto new_now = std::chrono::steady_clock::now();
        std::chrono::duration<double> frame_time = new_now - last_time;
        last_time = new_now;

        time_accumulator += frame_time.count();
        counter++;
        if (time_accumulator >= 1.0) {
            actual_rate = counter / time_accumulator;
            counter = 0;
            time_accumulator = 0.0;
        }
    }

    double getActualRate() const { return actual_rate; }

private:
    std::chrono::duration<double> target_duration;
    std::chrono::time_point<std::chrono::steady_clock> last_time;
    bool uncapped = false;
    bool high_precision_mode = true;

    int counter = 0;
    double time_accumulator = 0.0;
    double actual_rate = 0.0;
};

std::string helpString(const std::string& program_name)
{
    std::ostringstream oss;
    oss << "Usage: " << program_name << " n_numbers [options]\n\n"
        << "Spin a roulette with numbered entries and randomly select one.\n\n"
        << "Positional Arguments:\n"
        << "  n_numbers                Number of entries on the roulette (e.g. 10 for numbers 0~9)\n\n"
        << "Optional Arguments:\n"
        << "  -sz, --size <size>       Size of the roulette display in pixels (default: 50)\n"
        << "  -r,  --rounds <rounds>   Number of full circles to spin before stopping (default: 10)\n"
        << "  -st, --steps <steps>     Number of animation steps (smoothness/speed, default: 200)\n"
        << "  --text-color <hex>       Hex color code for text color (default: 000000)\n"
        << "  --highlight-color <hex>  Hex color code for highlight color (default: FF0000)\n"
        << "  --aa <mode>              Antialiasing mode: none, 2x, 4x, 8x, 16x (default: 4x)\n"
        << "  --max-fps <fps>          Maximum FPS limit for rendering (0 = uncapped, default: 60)\n"
        << "  --max-tps <tps>          Maximum TPS limit for logic updates (0 = uncapped, default: 100)\n"
        << "  --show-metrics           Show FPS/TPS stats in console output (default: off)\n"
        << "  --precise-timing         Enable high-precision timing using busy wait (default: off)\n"
        << "  -h,  --help              Show this help message and exit\n\n"
        << "Example:\n"
        << "  " << program_name << " 8 -sz 150 -r 20 -st 400 --aa 8x\n";
    return oss.str();
}

int main(int argc, char* argv[])
{
    ArgCLITool::ArgParser parser;
    parser.add("n_numbers");
    parser.add("-sz", "--size").nvalues(1).defaultValues({"50"});
    parser.add("-r", "--rounds").nvalues(1).defaultValues({"10"});
    parser.add("-st", "--steps").nvalues(1).defaultValues({"200"});
    parser.add("--text-color").nvalues(1).defaultValues({"000000"});
    parser.add("--highlight-color").nvalues(1).defaultValues({"FF0000"});
    parser.add("--aa").nvalues(1).defaultValues({"4x"});
    parser.add("--max-fps").nvalues(1).defaultValues({"60"});
    parser.add("--max-tps").nvalues(1).defaultValues({"100"});
    parser.add("--show-metrics");
    parser.add("--precise-timing");
    parser.add("-h", "--help");

    ArgCLITool::Args args;
    try {
        args = parser.parse(argc, argv);

        // Show help text and exit immediately if --help is provided
        if (args["-h"]) {
            std::cout << helpString(argv[0]) << std::endl;
            return 0;
        }

        // Extract and validate CLI arguments into global config
        config.n_numbers = args["n_numbers"].as<int>();
        config.angle = 2 * M_PI / config.n_numbers;
        config.radius = 1.0;
        config.size = args["-sz"].as<int>();
        config.rounds = args["-r"].as<int>();
        config.steps = args["-st"].as<int>();
        cm::RGB text_color = args["--text-color"].as<std::string>();
        config.text_color = {text_color.R, text_color.G, text_color.B};
        cm::RGB highlight_color = args["--highlight-color"].as<std::string>();
        config.highlight_color = {highlight_color.R, highlight_color.G, highlight_color.B};
        std::string aa_mode = args["--aa"].as<std::string>();
        bool unknown_aa_mode = false;
        if (aa_mode == "none") {
            config.aa_mode = q3::Rasterizer::AA_MODE::NONE;
        } else if (aa_mode == "2x") {
            config.aa_mode = q3::Rasterizer::AA_MODE::SSAA_2X;
        } else if (aa_mode == "4x") {
            config.aa_mode = q3::Rasterizer::AA_MODE::SSAA_4X;
        } else if (aa_mode == "8x") {
            config.aa_mode = q3::Rasterizer::AA_MODE::SSAA_8X;
        } else if (aa_mode == "16x") {
            config.aa_mode = q3::Rasterizer::AA_MODE::SSAA_16X;
        } else {
            unknown_aa_mode = true;
        }
        config.max_fps = args["--max-fps"].as<int>();
        config.max_tps = args["--max-tps"].as<int>();
        config.show_metrics = args["--show-metrics"];
        config.precise_timing = args["--precise-timing"];

        // Sanity check on user input values
        if (config.n_numbers <= 0) { throw std::invalid_argument("Number of entries must be greater than 0"); }
        if (config.size <= 0) { throw std::invalid_argument("Size must be greater than 0"); }
        if (config.rounds < 0) { throw std::invalid_argument("Number of rounds must be non-negative"); }
        if (config.steps <= 0) { throw std::invalid_argument("Number of steps must be greater than 0"); }
        if (unknown_aa_mode) { throw std::invalid_argument("Unknown antialiasing mode: " + aa_mode); }
        if (config.max_fps < 0) { throw std::invalid_argument("FPS limit must be non-negative"); }
        if (config.max_tps < 0) { throw std::invalid_argument("TPS limit must be non-negative"); }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << helpString(argv[0]) << std::endl;
        return 1;
    }

    // Allocate two framebuffers for double buffering
    // framebuffer_draw: used by logic thread to draw the next frame (back buffer)
    // framebuffer_render: currently displayed by render thread (front buffer)
    auto framebuffer_draw = std::make_shared<q3::GraphicsBuffer<q3::RGBColor>>(config.size, config.size);
    auto framebuffer_render = std::make_shared<q3::GraphicsBuffer<q3::RGBColor>>(config.size, config.size);

    // depthbuffer is shared by rasterizer (doesn't need double buffering)
    auto depthbuffer = std::make_shared<q3::GraphicsBuffer<float>>(config.size, config.size);

    // Initialize a roulette wheel with text labels (1 ~ n)
    Roulette roulette(config.n_numbers, config.radius, config.text_color, config.highlight_color, 50);

    // Randomly pick a final angle for the roulette to stop at
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0, 2 * M_PI);
    float stop_angle = dis(gen);

    // Initialize spin animation controller with stop angle and total steps
    RotationManager rotation_manager(stop_angle, config.steps);

    // Set up rasterizer for rendering the wheel (with AA settings)
    q3::Rasterizer rasterizer(framebuffer_draw, depthbuffer);
    rasterizer.setAntialiasingMode(config.aa_mode);

    // Configure the renderer to draw framebuffer to the screen
    Renderer renderer(config.size, config.size);

    // Initialize two rate timers:
    // - render_timer: caps the render thread to max_fps (0 = uncapped)
    // - logic_timer: caps the logic update loop to max_tps (0 = uncapped)
    RateTimer render_timer(config.max_fps, config.precise_timing);
    RateTimer logic_timer(config.max_tps, config.precise_timing);

    // Launch a render thread that continuously displays the front buffer
    std::atomic<bool> running = true;
    std::thread render_thread([&]() {
        while (running) {
            // Render the current front buffer to the console
            renderer.render();

            // Conditionally print metrics only if enabled
            if (config.show_metrics) {
                std::cout << "FPS/TPS: " << render_timer.getActualRate() << "/" << logic_timer.getActualRate() << std::endl;
            }

            // Wait until next frame based on FPS limit (0 = uncapped)
            render_timer.waitNext();
        }
    });

    while (!rotation_manager.step()) {
        // Update the roulette angle for this animation step
        roulette.setRotation(rotation_manager.getCurrentAngle());

        // Rasterizer will render into the back buffer (framebuffer_draw)
        rasterizer.setBuffers(framebuffer_draw, depthbuffer);

        // Clear the back buffer before drawing
        rasterizer.clearFrameBuffer({24, 24, 24, 0});
        rasterizer.clearDepthBuffer();

        // Render the scene into framebuffer_draw
        roulette.render(rasterizer);

        // Swap the back and front buffers
        // - framebuffer_draw becomes the new front buffer
        // - framebuffer_render becomes the new back buffer for the next frame
        std::swap(framebuffer_draw, framebuffer_render);

        // Tell the renderer to use the newly rendered buffer as the front buffer
        renderer.setBuffer(framebuffer_render);

        // Wait until next logic tick based on TPS limit (0 = uncapped)
        logic_timer.waitNext();
    }

    // Stop the render thread and wait for it to finish
    running = false;
    if (render_thread.joinable()) {
        render_thread.join();
    }
}
