import QtQuick 2.0

Item {
    property Item button : Item{
        property Item enabled: Item {
            property Gradient totalGradient: Gradient {
                GradientStop { position: 0.0; color: "#c6c6c6" }
                GradientStop { position: 1.0; color: "#aaaaaa" }
            }
            property Gradient topGradient: Gradient {
                GradientStop { position: 0.0; color: "#c6c6c6" }
                GradientStop { position: 1.0; color: "#b6b6b6" }
            }
            property Gradient middleGradient: Gradient {
                GradientStop { position: 0.0; color: "#c0c0c0" }
                GradientStop { position: 1.0; color: "#b0b0b0" }
            }
            property Gradient bottomGradient: Gradient {
                GradientStop { position: 0.0; color: "#b0b0b0" }
                GradientStop { position: 1.0; color: "#aaaaaa" }
            }
        }
        property Item disabled: Item {
            property Gradient totalGradient: Gradient {
                GradientStop { position: 0.0; color: "#797979" }
                GradientStop { position: 1.0; color: "#999999" }
            }
            property Gradient topGradient: Gradient {
                GradientStop { position: 0.0; color: "#797979" }
                GradientStop { position: 1.0; color: "#898989" }
            }
            property Gradient middleGradient: Gradient {
                GradientStop { position: 0.0; color: "#808080" }
                GradientStop { position: 1.0; color: "#909090" }
            }
            property Gradient bottomGradient: Gradient {
                GradientStop { position: 0.0; color: "#898989" }
                GradientStop { position: 1.0; color: "#999999" }
            }
        }

        property Item enabledHover: Item {
            property Gradient totalGradient: Gradient {
                GradientStop { position: 0.0; color: "#d6d6d6" }
                GradientStop { position: 1.0; color: "#bababa" }
            }
            property Gradient topGradient: Gradient {
                GradientStop { position: 0.0; color: "#d6d6d6" }
                GradientStop { position: 1.0; color: "#cacaca" }
            }
            property Gradient middleGradient: Gradient {
                GradientStop { position: 0.0; color: "#d0d0d0" }
                GradientStop { position: 1.0; color: "#c0c0c0" }
            }
            property Gradient bottomGradient: Gradient {
                GradientStop { position: 0.0; color: "#cacaca" }
                GradientStop { position: 1.0; color: "#bababa" }
            }
        }

        property Item pressed: Item {
            property Gradient totalGradient: Gradient {
                GradientStop { position: 0.0; color: "#373737" }
                GradientStop { position: 1.0; color: "#575757" }
            }
            property Gradient topGradient: Gradient {
                GradientStop { position: 0.0; color: "#373737" }
                GradientStop { position: 1.0; color: "#474747" }
            }
            property Gradient middleGradient: Gradient {
                GradientStop { position: 0.0; color: "#444444" }
                GradientStop { position: 1.0; color: "#545454" }
            }
            property Gradient bottomGradient: Gradient {
                GradientStop { position: 0.0; color: "#505050" }
                GradientStop { position: 1.0; color: "#575757" }
            }
        }
    }
    property Item comboBox : Item{
        property Item enabled: Item {
            property Gradient totalGradient: Gradient {
                GradientStop { position: 0.0; color: "#545454" }
                GradientStop { position: 1.0; color: "#373737" }
            }
            property Gradient topGradient: Gradient {
                GradientStop { position: 0.0; color: "#575757" }
                GradientStop { position: 1.0; color: "#444444" }
            }
            property Gradient middleGradient: Gradient {
                GradientStop { position: 0.0; color: "#474747" }
                GradientStop { position: 1.0; color: "#3c3c3c" }
            }
            property Gradient bottomGradient: Gradient {
                GradientStop { position: 0.0; color: "#444444" }
                GradientStop { position: 1.0; color: "#373737" }
            }
        }
        property Item disabled: Item {
            property Gradient totalGradient: Gradient {
                GradientStop { position: 0.0; color: "#797979" }
                GradientStop { position: 1.0; color: "#999999" }
            }
            property Gradient topGradient: Gradient {
                GradientStop { position: 0.0; color: "#797979" }
                GradientStop { position: 1.0; color: "#898989" }
            }
            property Gradient middleGradient: Gradient {
                GradientStop { position: 0.0; color: "#808080" }
                GradientStop { position: 1.0; color: "#909090" }
            }
            property Gradient bottomGradient: Gradient {
                GradientStop { position: 0.0; color: "#898989" }
                GradientStop { position: 1.0; color: "#999999" }
            }
        }

        property Item enabledHover: Item {
            property Gradient totalGradient: Gradient {
                GradientStop { position: 0.0; color: "#737373" }
                GradientStop { position: 1.0; color: "#474747" }
            }
            property Gradient topGradient: Gradient {
                GradientStop { position: 0.0; color: "#737373" }
                GradientStop { position: 1.0; color: "#626262" }
            }
            property Gradient middleGradient: Gradient {
                GradientStop { position: 0.0; color: "#666666" }
                GradientStop { position: 1.0; color: "#585858" }
            }
            property Gradient bottomGradient: Gradient {
                GradientStop { position: 0.0; color: "#5e5e5e" }
                GradientStop { position: 1.0; color: "#474747" }
            }
        }

        property Item pressed: Item {
            property Gradient totalGradient: Gradient {
                GradientStop { position: 0.0; color: "#373737" }
                GradientStop { position: 1.0; color: "#575757" }
            }
            property Gradient topGradient: Gradient {
                GradientStop { position: 0.0; color: "#373737" }
                GradientStop { position: 1.0; color: "#484848" }
            }
            property Gradient middleGradient: Gradient {
                GradientStop { position: 0.0; color: "#424242" }
                GradientStop { position: 1.0; color: "#525252" }
            }
            property Gradient bottomGradient: Gradient {
                GradientStop { position: 0.0; color: "#505050" }
                GradientStop { position: 1.0; color: "#575757" }
            }
        }
        property Item pressedHover: Item {
            property Gradient totalGradient: Gradient {
                GradientStop { position: 0.0; color: "#474747" }
                GradientStop { position: 1.0; color: "#676767" }
            }
            property Gradient topGradient: Gradient {
                GradientStop { position: 0.0; color: "#474747" }
                GradientStop { position: 1.0; color: "#585858" }
            }
            property Gradient middleGradient: Gradient {
                GradientStop { position: 0.0; color: "#525252" }
                GradientStop { position: 1.0; color: "#626262" }
            }
            property Gradient bottomGradient: Gradient {
                GradientStop { position: 0.0; color: "#606060" }
                GradientStop { position: 1.0; color: "#676767" }
            }
        }
    }
    property Item checkBox : Item{
        property Item enabled: Item {
            property Gradient totalGradient: Gradient {
                GradientStop { position: 0.0; color: "#737373" }
                GradientStop { position: 1.0; color: "#474747" }
            }
            property Gradient topGradient: Gradient {
                GradientStop { position: 0.0; color: "#737373" }
                GradientStop { position: 1.0; color: "#626262" }
            }
            property Gradient middleGradient: Gradient {
                GradientStop { position: 0.0; color: "#666666" }
                GradientStop { position: 1.0; color: "#585858" }
            }
            property Gradient bottomGradient: Gradient {
                GradientStop { position: 0.0; color: "#5e5e5e" }
                GradientStop { position: 1.0; color: "#474747" }
            }
        }
        property Item disabled: Item {
            property Gradient totalGradient: Gradient {
                GradientStop { position: 0.0; color: "#797979" }
                GradientStop { position: 1.0; color: "#999999" }
            }
            property Gradient topGradient: Gradient {
                GradientStop { position: 0.0; color: "#797979" }
                GradientStop { position: 1.0; color: "#898989" }
            }
            property Gradient middleGradient: Gradient {
                GradientStop { position: 0.0; color: "#808080" }
                GradientStop { position: 1.0; color: "#909090" }
            }
            property Gradient bottomGradient: Gradient {
                GradientStop { position: 0.0; color: "#898989" }
                GradientStop { position: 1.0; color: "#999999" }
            }
        }

        property Item enabledHover: Item {
            property Gradient totalGradient: Gradient {
                GradientStop { position: 0.0; color: "#838383" }
                GradientStop { position: 1.0; color: "#575757" }
            }
            property Gradient topGradient: Gradient {
                GradientStop { position: 0.0; color: "#838383" }
                GradientStop { position: 1.0; color: "#727272" }
            }
            property Gradient middleGradient: Gradient {
                GradientStop { position: 0.0; color: "#767676" }
                GradientStop { position: 1.0; color: "#686868" }
            }
            property Gradient bottomGradient: Gradient {
                GradientStop { position: 0.0; color: "#6e6e6e" }
                GradientStop { position: 1.0; color: "#575757" }
            }
        }

        property Item pressed: Item {
            property Gradient totalGradient: Gradient {
                GradientStop { position: 0.0; color: "#373737" }
                GradientStop { position: 1.0; color: "#575757" }
            }
            property Gradient topGradient: Gradient {
                GradientStop { position: 0.0; color: "#373737" }
                GradientStop { position: 1.0; color: "#484848" }
            }
            property Gradient middleGradient: Gradient {
                GradientStop { position: 0.0; color: "#424242" }
                GradientStop { position: 1.0; color: "#525252" }
            }
            property Gradient bottomGradient: Gradient {
                GradientStop { position: 0.0; color: "#505050" }
                GradientStop { position: 1.0; color: "#575757" }
            }
        }
        property Item pressedHover: Item {
            property Gradient totalGradient: Gradient {
                GradientStop { position: 0.0; color: "#474747" }
                GradientStop { position: 1.0; color: "#676767" }
            }
            property Gradient topGradient: Gradient {
                GradientStop { position: 0.0; color: "#474747" }
                GradientStop { position: 1.0; color: "#585858" }
            }
            property Gradient middleGradient: Gradient {
                GradientStop { position: 0.0; color: "#525252" }
                GradientStop { position: 1.0; color: "#626262" }
            }
            property Gradient bottomGradient: Gradient {
                GradientStop { position: 0.0; color: "#606060" }
                GradientStop { position: 1.0; color: "#676767" }
            }
        }
    }
    property Item inputField : Item {
        property Item enabled: Item {
            property Gradient totalGradient: Gradient {
                GradientStop { position: 0.0; color: "#e6e6e6" }
                GradientStop { position: 1.0; color: "#cacaca" }
            }
            property Gradient topGradient: Gradient {
                GradientStop { position: 0.0; color: "#e6e6e6" }
                GradientStop { position: 1.0; color: "#dadada" }
            }
            property Gradient middleGradient: Gradient {
                GradientStop { position: 0.0; color: "#e0e0e0" }
                GradientStop { position: 1.0; color: "#d0d0d0" }
            }
            property Gradient bottomGradient: Gradient {
                GradientStop { position: 0.0; color: "#dadada" }
                GradientStop { position: 1.0; color: "#cacaca" }
            }
        }

        property Item enabledHover: Item {
            property Gradient totalGradient: Gradient {
                GradientStop { position: 0.0; color: "#D0D0D0" }
                GradientStop { position: 1.0; color: "#F0F0F0" }
            }
            property Gradient topGradient: Gradient {
                GradientStop { position: 0.0; color: "#D0D0D0" }
                GradientStop { position: 1.0; color: "#E0E0E0" }
            }
            property Gradient middleGradient: Gradient {
                GradientStop { position: 0.0; color: "#D8D8D8" }
                GradientStop { position: 1.0; color: "#E8E8E8" }
            }
            property Gradient bottomGradient: Gradient {
                GradientStop { position: 0.0; color: "#E0E0E0" }
                GradientStop { position: 1.0; color: "#F0F0F0" }
            }
        }
        property Item disabled: Item {
            property Gradient totalGradient: Gradient {
                GradientStop { position: 0.0; color: "#797979" }
                GradientStop { position: 1.0; color: "#999999" }
            }
            property Gradient topGradient: Gradient {
                GradientStop { position: 0.0; color: "#797979" }
                GradientStop { position: 1.0; color: "#898989" }
            }
            property Gradient middleGradient: Gradient {
                GradientStop { position: 0.0; color: "#808080" }
                GradientStop { position: 1.0; color: "#909090" }
            }
            property Gradient bottomGradient: Gradient {
                GradientStop { position: 0.0; color: "#898989" }
                GradientStop { position: 1.0; color: "#999999" }
            }
        }

        property Item pressed: Item {
            property Gradient totalGradient: Gradient {
                GradientStop { position: 1.0; color: "#e6e6e6" }
                GradientStop { position: 0.0; color: "#cacaca" }
            }
            property Gradient topGradient: Gradient {
                GradientStop { position: 1.0; color: "#dadada" }
                GradientStop { position: 0.0; color: "#cacaca" }
            }
            property Gradient middleGradient: Gradient {
                GradientStop { position: 1.0; color: "#e0e0e0" }
                GradientStop { position: 0.0; color: "#d0d0d0" }
            }
            property Gradient bottomGradient: Gradient {
                GradientStop { position: 1.0; color: "#e6e6e6" }
                GradientStop { position: 0.0; color: "#dadada" }
            }
        }
    }
}
