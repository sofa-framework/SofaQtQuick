namespace sofa
{
namespace qtquick
{

#include "AssetFactory.h"

template <class T> AssetCreator<T>::AssetCreator() {}

template <class T> AssetCreator<T>::~AssetCreator() {}

template <class T>
std::shared_ptr<Asset> AssetCreator<T>::createInstance(std::string path,
                                            std::string extension)
{
    return std::shared_ptr<T>(new T(path, extension));
}

} // namespace qtquick
} // namespace sofa
