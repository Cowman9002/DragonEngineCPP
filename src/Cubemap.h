//#pragma once
//
//#include <string>
//
//#include "textureEnums.h"
//
//namespace dgn
//{
//    class Cubemap
//    {
//        friend class Renderer;
//    private:
//        unsigned m_texture;
//        unsigned m_widths[6], m_heights[6];
//    public:
//
//        Cubemap();
//
//        void dispose();
//
//        /**
//            Order:
//            East
//            West
//            Up
//            Down
//            North
//            South
//        */
//
//        Cubemap& createFromData(const void *data[6], TextureData data_type, unsigned width[6], unsigned height[6],
//                                TextureWrap wrap, TextureFilter filter,
//                                TextureStorage internal_storage, TextureStorage given_storage);
//
//        Cubemap& createFromData(const void *data[6], TextureData data_type, unsigned width, unsigned height,
//                                TextureWrap wrap, TextureFilter filter,
//                                TextureStorage internal_storage, TextureStorage given_storage);
//
//        Cubemap& loadFromFiles(std::string filepath[6], TextureWrap wrap, TextureFilter filter,
//                               TextureStorage internal_storage);
//
//        /** Individual file names must be N.png, E.png, S.png, W.png, U.png, D.png**/
//        Cubemap& loadFromDirectory(std::string filepath, TextureWrap wrap, TextureFilter filter,
//                               TextureStorage internal_storage);
//
//        unsigned getNativeTexture();
//        unsigned getWidth(unsigned f = 0);
//        unsigned getHeight(unsigned f = 0);
//
//        static void setWrap(TextureWrap wrap);
//        static void setFilter(TextureFilter filter);
//        static void generateMipmaps();
//    };
//}
