#ifndef _PAPER_CONSTANTS__
#define _PAPER_CONSTANTS__

 #define PAPER_SIZE 23
//#define PAPER_SIZE 46

class PaperConstants
{
public:
	static const int PAGE_DPI = 300;
#if PAPER_SIZE == 23
	static inline float PAGE_WIDTH() { return 153.0f; }//inches
	static inline float PAGE_HEIGHT() { return  244.0f; } //inches
	static inline std::string FACE_IMAGE() { return ":/img/face23"; }
    static inline std::string PRINT_LAYER() { return ":/print/layer23"; }
#elif PAPER_SIZE == 46
    static inline float PAGE_WIDTH() { return 100.0f; } //inches
    static inline float PAGE_HEIGHT() { return 148.0f; } //inches
    static inline std::string FACE_IMAGE() { return ":/img/face46"; }
    static inline std::string PRINT_LAYER() { return ":/print/layer46"; }
#else
    #error undefined paper size
#endif
	static inline float ASPECT_RATIO() { return PAGE_WIDTH() / PAGE_HEIGHT(); }

    inline static void Scale(const float w, const float h, float &frameW, float &frameH)
    {
    	const float scaleW=w/PAGE_WIDTH();
    	const float scaleH=h/PAGE_HEIGHT();

    	const float scale = std::min(scaleW,scaleH);

    	frameW=PAGE_WIDTH()*scale;
    	frameH=PAGE_HEIGHT()*scale;
    }
};

#endif //_PAPER_CONSTANTS__
