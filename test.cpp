#include "hevc_enc/vp_hevc_codec_1_0.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

typedef struct codec_context {
    int width;
    int height;
    int framerate;
    int bitrate;
    int frame_count;
    vl_img_format_t input_format;
    int gop;

    vl_codec_handle_t encoder;

    unsigned char *inputBuffer;
    unsigned char *outputBuffer;
    unsigned int outputBufferLen;
} codec_ctx;

int main(int argc, const char *argv[]){
    codec_ctx ctx = { 0 };
    int outfd = -1;
    FILE *fp = NULL;
    int datalen = 0;

    if (argc < 9)
    {
        printf("Amlogic HEVC Encode API \n");
        printf(" usage: output [srcfile][outfile][width][height][gop][framerate][bitrate][num]\n");
        printf("  options  :\n");
        printf("  srcfile  : yuv data url in your root fs\n");
        printf("  outfile  : stream url in your root fs\n");
        printf("  width    : width\n");
        printf("  height   : height\n");
        printf("  gop      : I frame refresh interval\n");
        printf("  framerate: framerate \n ");
        printf("  bitrate  : bit rate \n ");
        printf("  num      : encode frame count \n ");
        printf("  fmt      : encode input fmt 1:nv12, 2:nv21\n");
        return -1;
    }
    else
    {
        printf("%s\n", argv[1]);
        printf("%s\n", argv[2]);
    }
    ctx.width =  atoi(argv[3]);
    if ((ctx.width < 1) || (ctx.width > 3840/*1920*/))
    {
        printf("invalid width \n");
        return -1;
    }
    ctx.height = atoi(argv[4]);
    if ((ctx.height < 1) || (ctx.height > 2160/*1080*/))
    {
        printf("invalid height \n");
        return -1;
    }
    ctx.gop = atoi(argv[5]);
    ctx.framerate = atoi(argv[6]);
    ctx.bitrate = atoi(argv[7]);
    ctx.frame_count = atoi(argv[8]);
    ctx.input_format = (vl_img_format_t)atoi(argv[9]);

    if ((ctx.framerate < 0) || (ctx.framerate > 30))
    {
        printf("invalid framerate \n");
        return -1;
    }
    if (ctx.bitrate <= 0)
    {
        printf("invalid bitrate \n");
        return -1;
    }
    if (ctx.frame_count < 0)
    {
        printf("invalid num \n");
        return -1;
    }
    printf("src_url is: %s ;\n", argv[1]);
    printf("out_url is: %s ;\n", argv[2]);
    printf("width   is: %d ;\n", ctx.width);
    printf("height  is: %d ;\n", ctx.height);
    printf("gop     is: %d ;\n", ctx.gop);
    printf("frmrate is: %d ;\n", ctx.framerate);
    printf("bitrate is: %d ;\n", ctx.bitrate);
    printf("frm_num is: %d ;\n", ctx.frame_count);

    unsigned int frameSize  = ctx.width * ctx.height * 3 / 2;
    ctx.outputBufferLen = 1024 * 1024 * sizeof(char);
    ctx.inputBuffer = (unsigned char *)malloc(frameSize);
    ctx.outputBuffer = (unsigned char *)malloc(ctx.outputBufferLen);

    fp = fopen((char *)argv[1], "rb");
    if (fp == NULL)
    {
        printf("open src file error!\n");
        goto exit;
    }
    outfd = open((char *)argv[2], O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (outfd < 0)
    {
        printf("open dist file error!\n");
        goto exit;
    }
    ctx.encoder = vl_video_encoder_init(CODEC_ID_H265, ctx.width, ctx.height, ctx.framerate, ctx.bitrate, ctx.gop);
    while (ctx.frame_count > 0) {
        if (fread(ctx.inputBuffer, 1, frameSize, fp) != frameSize) {
            printf("read input file error!\n");
            break;
        }
        memset(ctx.outputBuffer, 0, ctx.outputBufferLen);
        datalen = vl_video_encoder_encode(ctx.encoder, FRAME_TYPE_AUTO, ctx.inputBuffer, ctx.outputBufferLen, ctx.outputBuffer, ctx.input_format);
        if (datalen >= 0)
            write(outfd, (unsigned char *)ctx.outputBuffer, datalen);
        else {
            printf("encode error %d! continue ?\n",datalen);
            //break;
        }
        ctx.frame_count--;
    }
    vl_video_encoder_destroy(ctx.encoder);
    close(outfd);
    fclose(fp);
    free(ctx.inputBuffer);
    free(ctx.outputBuffer);
    return 0;
exit:
    if (ctx.inputBuffer)
        free(ctx.inputBuffer);
    if (ctx.outputBuffer)
        free(ctx.outputBuffer);
    if (outfd >= 0)
        close(outfd);
    if (fp)
        fclose(fp);
    return -1;
}
