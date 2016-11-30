#ifndef _PAPER_CONSTANTS__
#define _PAPER_CONSTANTS__

#define PAPER_SIZE 23
// #define PAPER_SIZE 46

class PaperConstants
{
public:
	constexpr static const int PAGE_DPI = 300;
#if PAPER_SIZE == 23
    constexpr static const float PAGE_WIDTH  = 2.1f; //inches
    constexpr static const float PAGE_HEIGHT = 3.4f; //inches
    constexpr static const char* FACE_IMAGE  = ":/img/face23";
#elif PAPER_SIZE == 46
    constexpr static const float PAGE_WIDTH  = 4.0f; //inches
    constexpr static const float PAGE_HEIGHT = 6.0f; //inches
    constexpr static const char* FACE_IMAGE  = ":/img/face46";
#else
    #error undefined paper size
#endif
    constexpr static const float ASPECT_RATIO = PAGE_WIDTH/PAGE_HEIGHT;

    inline static void Scale(const float w, const float h, float &frameW, float &frameH)
    {
    	const float scaleW=w/PAGE_WIDTH;
    	const float scaleH=h/PAGE_HEIGHT;

    	const float scale = std::min(scaleW,scaleH);

    	frameW=PAGE_WIDTH*scale;
    	frameH=PAGE_HEIGHT*scale;
    }
};

#endif //_PAPER_CONSTANTS__
