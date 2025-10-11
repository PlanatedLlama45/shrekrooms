/*
 * UNUSED FILE
*/

#pragma once

#include "imports.hpp"
#include "glc.hpp"

namespace shrekrooms::gl {

// Only square bitmap with 2^n size [32; 1024]
struct BitmapFont {
public:
    BitmapFont(const GLContext &glc, const std::string &bitmapPath, const std::string &dataPath, bool bitmapInterpolation = true) :
            m_glc(glc), m_uniman(glc.getUniformManager()), m_bitmap(bitmapPath, bitmapInterpolation) {
        // Bitmap
        m_bitmapSize = m_bitmap.getSize();

        if (m_bitmapSize.x != m_bitmapSize.y)
            throw error { "gl.hpp", "shrekrooms::gl::BitmapFont::BitmapFont", "Bitmap must be square" };

        switch (m_bitmapSize.x) {
            case 32: case 64: case 128: case 256: case 512: case 1024:
                break;
            default:
                throw error { "gl.hpp", "shrekrooms::gl::BitmapFont::BitmapFont", "Bitmap must be 2^n size [128; 1024]" };
        }

        // Data
        m_loadFontData(dataPath);
    }

    ~BitmapFont() {
        for (M_Character &ch : m_charset){
            glDeleteVertexArrays(1, &ch.vao);
            glDeleteBuffers(1, &ch.vbo);
        }
    }

    // TODO: add 'Align' instead of 'bool center'
    void render(const std::string &text, const glm::vec2 &pos, float charSize, const gl::Color &color = { 0.0f, 0.0f, 0.0f }, bool center = true) const {

        m_uniman.beginTextRender();
        m_uniman.useTexture(m_bitmap);
        m_uniman.setColor(color);

        std::string line;
        float lineNum = 0;
        glm::vec2 decr { 0.0f, charSize };
        std::size_t l = 0, r;
        for (r = 0; r < text.length(); r++) {
            if (text[r] == '\n') {
                line = text.substr(l, r-l);
                m_renderLine(line, pos - decr*lineNum, charSize, center);
                lineNum += 1.0f;
                l = r+1;
            }
        }

        line = text.substr(l, r-l);
        m_renderLine(line, pos - decr*lineNum, charSize, center);

        m_uniman.endTextRender();
    }

protected:
    struct M_Character {
        char value;
        glm::vec2 texCoordMin;
        glm::vec2 texCoordMax;
        GLuint vao, vbo;
    };


    const GLContext &m_glc;
    const UniformManager &m_uniman;
    Texture m_bitmap;
    glm::ivec2 m_bitmapSize;
    std::vector<M_Character> m_charset;
    

    void m_loadFontData(const std::string &dataPath) {
        std::ifstream file(dataPath);

        if (!file.is_open())
            throw error { "gl.hpp", "shrekrooms::gl::BitmapFont::m_loadFontData", "Failed to load font file" };
        
        std::string line;
        std::getline(file, line);
        if (line == "monospace") {
            /*
                Monospace file schema:
                `
                monospace
                [character count: integer >0]
                [characters per line: integer 2^n]
                [characters: ex. "abcdefg", no quotes]
                `
            */

            std::getline(file, line);
            size_t charCount = std::stoull(line);
            m_charset.reserve(charCount);

            std::getline(file, line);
            size_t charsPerLine = std::stoull(line);

            std::getline(file, line);
            if (line.length() != charCount)
                throw error { "gl.hpp", "shrekrooms::gl::BitmapFont::m_loadFontData", "Character count must match charset length: expected " } << std::to_string(charCount) << ", got " << std::to_string(line.length());

            float charSizeTex = 1.0f / charsPerLine;
            glm::vec2 charSizeTexVec { charSizeTex, charSizeTex };
            M_Character ch;
            for (size_t i = 0; i < charCount; i++) {
                ch.value = line[i];
                ch.texCoordMin = glm::vec2 { charSizeTex * (i%charsPerLine), charSizeTex * (i/charsPerLine) };
                ch.texCoordMax = ch.texCoordMin + charSizeTexVec;
                auto vaovbo = m_initGeometry(ch.texCoordMin, ch.texCoordMax);
                ch.vao = vaovbo.first;
                ch.vbo = vaovbo.second;
                m_charset.push_back(ch);
            }

        } else if (line == "non_monospace")
            throw error { "gl.hpp", "shrekrooms::gl::BitmapFont::m_loadFontData", "Non-monospace fonts not implemented yet" };
        else
            throw error { "gl.hpp", "shrekrooms::gl::BitmapFont::m_loadFontData", "Unknown font format: '" } << line << "'";

        file.close();
    }

    // ret: { vao, vbo }
    std::pair<GLuint, GLuint> m_initGeometry(glm::vec2 texCoordMin, glm::vec2 texCoordMax) {
        GLuint vao, vbo;
        float verts[30] = {
            1.0f, 1.0f, 0.0f,  texCoordMax.x, texCoordMin.y,
            1.0f, 0.0f, 0.0f,  texCoordMax.x, texCoordMax.y,
            0.0f, 1.0f, 0.0f,  texCoordMin.x, texCoordMin.y,

            0.0f, 1.0f, 0.0f,  texCoordMin.x, texCoordMin.y,
            1.0f, 0.0f, 0.0f,  texCoordMax.x, texCoordMax.y,
            0.0f, 0.0f, 0.0f,  texCoordMin.x, texCoordMax.y
        };

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 30*4, verts, GL_STATIC_DRAW);
        // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*4, (void*)0);
        glEnableVertexAttribArray(0);
        // Texture coordinates
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*4, (void*)(3*4));
        glEnableVertexAttribArray(1);
        
        // P.S. sizeof(float) = 4

        return { vao, vbo };
    }

    void m_renderLine(const std::string &line, const glm::vec2 &pos, float charSize, bool center) const {
        static glm::vec2 winSize = m_glc.getWindow().getSizeVector();

        glm::vec2 charPos = 2.0f * pos / winSize;
        glm::vec2 size;
        if (center)
            charPos.x -= charSize * line.length() / winSize.x;

        glm::mat4 translateMat, scaleMat;

        for (char ch : line) {
            translateMat = glm::translate(glm::mat4(1.0f), { charPos, 0.0f });

            auto charIterator = std::find_if(m_charset.begin(), m_charset.end(), [&ch](M_Character chIter) { return (chIter.value == ch); } );
            if (charIterator == m_charset.end())
                throw error { "gl.hpp", "shrekrooms::gl::BitmapFont::render", "Character '" } << ch << "' is not present in the font charset";

            M_Character char_ = *charIterator;

            size = 2.0f * charSize / winSize;
            scaleMat = glm::scale(glm::mat4(1.0f), { size, 1.0f });
/*
            m_uniman.setTranslateMatrix(translateMat);
            m_uniman.setScaleMatrix(scaleMat);
*/
            
            glBindVertexArray(char_.vao);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            charPos.x += size.x;
        }
    }
};


} // namespace shrekrooms::gl
