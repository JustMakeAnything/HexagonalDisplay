struct CRGBW {
	union {
		struct {
            union {
                uint8_t r;
                uint8_t red;
            };
            union {
                uint8_t g;
                uint8_t green;
            };
            union {
                uint8_t b;
                uint8_t blue;
            };
            union {
                uint8_t w;
                uint8_t white;
            };
        };
		uint8_t raw[4];
	};
   
    // Constructor for r,g,b,w values
    inline CRGBW( uint8_t ir, uint8_t ig, uint8_t ib, uint8_t iw) __attribute__((always_inline))
        : r(ir),g(ib), b(ig),  w(iw)
    {}	
   
    // Constructor for 32-bit hex 0xRRGGBBWW color code
    inline CRGBW( uint32_t colorcode) __attribute__((always_inline))
    : r((colorcode >> 0) & 0xFF), g((colorcode >> 8) & 0xFF),b((colorcode >> 16) & 0xFF),w((colorcode >> 24) & 0xFF)
    {}
    
};

CRGBW& nblend(CRGBW& existing, const CRGBW& overlay, uint8_t amountOfOverlay );
