## Note: The file format might change in any way.
## This experimental github project is a just test bed for demonstrating how spatial gene data could be stored together with some microscope photos.

To try the file format out, first install this software and then create st_exp_protobuf files by downloading and converting [online research data](http://www.spatialtranscriptomicsresearch.org/datasets/). The shell script sh/download_and_convert_example_data.sh automates the download and conversion.

st_exp_protobuf files can be viewed with a web viewer from [osd-spot-viewer](https://github.com/eriksjolund/osd-spot-viewer).

Design goal:

- The file format should be efficient for both local file usage and usage over a network. Network usage is a bit different
as then the size of the downloaded data and also the number of read round-trips get to be important.
If only information of one gene or one spot is needed, preferably only few reads (few round-trips) should be needed and preferably only the required information would be downloaded. In other words, you shouldn't have to download the whole file
if you are only interested in one specific gene.
(Consider that AWS charge per downloaded Gb)

- The header should be small. That is important if a web browser would like to download headers from many experiments to
be able to present an overview of all experiments. (Improve this, the current header could get to be a bit smaller)

- c++, python and javascript parsing and serialization examples should be included

A note about the choice of serialization library:

Protobuf version 2 was chosen as it is a well-proven library, used by many for a long time.
Two newer competitors, [capnproto](https://capnproto.org/) and [flatbuffers](http://google.github.io/flatbuffers/) were considered but they were not chosen because

- capnproto, although being very high quality software, is being labeled as beta software and the javascript project [capnp-js](https://github.com/capnp-js/serialization) has had no commits since  Feb 27 2015 (today's date is May 19 2016).

- the javascript support for flatbuffers is not quite there yet. The javascript fuzz test fails as of May 19 2016. See [support for different programming languages](http://google.github.io/flatbuffers/flatbuffers_support.html).


## Installation (on Ubuntu 16.04)

    sudo apt-get install libyajl-dev libyajl2
    sudo apt-get install cmake ninja-build

Download and install Qt5.7. 

    mkdir /tmp/build
    cd /tmp/build
    /home/user/cmake-3.4.0-Linux-x86_64/bin/cmake "-DCMAKE_PREFIX_PATH=/home/user/Qt5.6.0/5.6/gcc_64" /path/to/st_exp_protobuf
    make

or if you use Ninja as make-tool:

    mkdir /tmp/build
    cd /tmp/build
    cmake -G Ninja "-DCMAKE_PREFIX_PATH=/home/user/Qt/5.7/gcc_64"  ~/st_exp_protobuf/
    make
