Design goal:

- The file format should be efficient for both local file usage and usage over a network. Network usage is a bit different
as then the size of the downloaded data and also the number of read round-trips get to be important.
If only information of one gene or one spot is needed, preferably only few reads (few round-trips) should be needed and preferably only the required information would be downloaded. In other words, you shouldn't have to download the whole file
if you are only interested in one specific gene.
(Consider that AWS charge per downloaded Gb)

- The header should be small. Especially important if a web browser would like to download headers from many experiments to
be able to present an overview of all experiments. (improve this, the current header could get to be a bit smaller)

- c++, python and javascript parsing and serialization examples should be included

A note about the choice of serialization library.

Protobuf version 2 has been a well-proven library and used for a long time.
Two newer competitors, [capnproto](https://capnproto.org/) and [flatbuffers](http://google.github.io/flatbuffers/) were considered but they were not choosen because

- capnproto, although being very high quality software, is being labeled as beta software and the javascript project [capnp-js](https://github.com/capnp-js/serialization) has had no commits since  Feb 27 2015 (today's date is May 19 2016).

- the javascript support for flatbuffers is not quite there yet. The javascript fuzz test fails as of May 19 2016. See [support for different programming languages](http://google.github.io/flatbuffers/flatbuffers_support.html).
