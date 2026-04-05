#pragma once
#include <GL/glew.h>
#include <cstdio>

class ShadowMap {
private:
    unsigned int depthMapFBO;
    unsigned int depthMapTexture;
    int width;
    int height;

public:
    ShadowMap() : depthMapFBO(0), depthMapTexture(0), width(1024), height(1024) {}

    bool init(int shadowWidth = 1024, int shadowHeight = 1024) {
        width = shadowWidth;
        height = shadowHeight;

        glGenFramebuffers(1, &depthMapFBO);
        glGenTextures(1, &depthMapTexture);

        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0,
            GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            printf("ShadowMap FBO error!\n");
            return false;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        printf("ShadowMap initialized!\n");
        return true;
    }

    void bindForWriting() {
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void bindForReading(int textureUnit = 1) {
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    }

    void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
    bool isValid() const { return depthMapFBO != 0; }
    unsigned int getTextureId() const { return depthMapTexture; }
};