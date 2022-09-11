import typing
if typing.TYPE_CHECKING:
    from ..CutsceneTypes import Cutscene, Target, TargetType
else:
    from CutsceneTypes import Cutscene, Target, TargetType


def cutscene(c: Cutscene):
    c.set_animation(Target(TargetType.SPRITE, 0), "upMove")
    c.set_pos_in_frames(Target(TargetType.SPRITE, 0), 127, 62, 120)
    c.set_collider_enabled(2, False)
    c.wait_frames(120)
    c.set_animation(Target(TargetType.SPRITE, 0), "downIdle")
