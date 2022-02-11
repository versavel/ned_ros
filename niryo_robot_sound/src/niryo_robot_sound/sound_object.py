# Lib
import os
import subprocess
import rospy

from niryo_robot_sound.msg import SoundObject


class SoundException(Exception):
    pass


class SoundFormatException(Exception):
    pass


class SoundFileException(Exception):
    pass


class Sound(object):

    def __init__(self, name, path):
        self.__name = name
        self.__path = path

        rospy.logdebug("{} | {}".format(self.__name, self.__path))
        if not self.exists():
            raise SoundFileException("File {} doesn't exists".format(self.__path))

        self.__duration = self.__get_sound_duration()

    def __str__(self):
        return self.__name

    def __repr__(self):
        return self.__str__()

    @property
    def name(self):
        return self.__name

    @property
    def path(self):
        return self.__path

    @property
    def duration(self):
        return self.__duration

    def to_msg(self):
        return SoundObject(self.__name, self.__duration)

    def exists(self):
        return os.path.exists(self.__path)

    def __get_sound_duration(self):
        args = ("ffprobe", "-loglevel", "quiet", "-show_entries", "format=duration", "-i", self.__path)
        popen = subprocess.Popen(args, stdout=subprocess.PIPE)
        popen.wait()
        output = popen.stdout.read()
        try:
            return float(output.replace("[FORMAT]\nduration=", "").replace("\n[/FORMAT]\n", ""))
        except ValueError:
            raise SoundFormatException()
