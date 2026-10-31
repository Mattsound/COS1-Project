#include "MazePresets.h"

const std::vector<MazePresets::Preset>& MazePresets::getAll()
{
    static const std::vector<Preset> presets =
    {
        {
            "Small (9 x 7)",
            {
                "#########",
                "#S  #   #",
                "# # # # #",
                "# #   # #",
                "# ### # #",
                "#   #  G#",
                "#########",
            }
        },
        {
            "Medium (21 x 11)",
            {
                "#####################",
                "#S    #       #     #",
                "# ### # ##### # ### #",
                "# #   #     # #   # #",
                "# # ####### # ### # #",
                "# #       # #     # #",
                "# ####### # ####### #",
                "#       #         # #",
                "####### # ####### # #",
                "#         #        G#",
                "#####################",
            }
        },
        {
            "Sealed Vault (no path)",
            {
                "###########",
                "#S   #    #",
                "# ## # ## #",
                "#    #  G #",
                "###########",
            }
        },
    };

    return presets;
}
