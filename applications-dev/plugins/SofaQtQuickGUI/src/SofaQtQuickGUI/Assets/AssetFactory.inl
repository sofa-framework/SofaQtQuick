#include <QSharedPointer>

namespace sofaqtquick
{

#include "AssetFactory.h"

template <class T> AssetCreator<T>::AssetCreator() {}

template <class T> AssetCreator<T>::~AssetCreator() {}

template <class T>
QSharedPointer<Asset> AssetCreator<T>::createInstance(std::string path,
                                            std::string extension)
{
    return QSharedPointer<Asset>(new T(path, extension));
}

} // namespace sofaqtquick
