/*
Copyright 2018,
Author: damien.marchal@univ-lille1.fr, Copyright 2016 CNRS.

This file is part of Sofa

Sofa is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sofaqtquick is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Sofa. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef SOFAQTQUICKGUI_HELPER_QMATHEXTENSION_H
#define SOFAQTQUICKGUI_HELPER_QMATHEXTENSION_H

#include <QMatrix4x4>

namespace sofaqtquickgui
{

namespace helper
{

class QMath
{
public:
    template<class M>
    static const M& Zero() { static M m;
                             m.fill(0.0);
                                                  return m; }

    template<class M>
    static const M& Identity() { static M m;
                                 return m; }

    static void setMatrixFrom( QMatrix4x4& dest, const double* dmat )
    {
        QMatrix4x4 tmp(dmat[0], dmat[1], dmat[2], dmat[3]
                , dmat[4], dmat[5], dmat[6], dmat[7]
                , dmat[8], dmat[9], dmat[10], dmat[11]
                , dmat[12], dmat[13], dmat[14], dmat[15]);

        dest = tmp;
    }
};

}

}

#endif // SOFAQTQUICKGUI_HELPER_QMATHEXTENSION_H
