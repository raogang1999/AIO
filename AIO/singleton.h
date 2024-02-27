#ifndef AIO_SINGLETON_H
#define AIO_SINGLETON_H

namespace AIO {

    template<class T, class X = void, int N = 0>
    class Singleton {
    public:
        static T *GetInstance() {
            static T v;
            return &v;
        }
    };

    template<class T, class X = void, int N = 0>
    class SingletonPtr {
    public:
        static std::shared_ptr<T> GetInstance() {
            static std::shared_ptr<T> v(new T);
            return v;
        }
    };


} // namespace AIO


#endif