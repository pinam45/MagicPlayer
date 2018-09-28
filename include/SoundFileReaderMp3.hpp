////////////////////////////////////////////////////////////
//
// SFML Extension - SoundFileReaderMp3
// Copyright (C) 2018 Maxime Pinard
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#ifndef SFML_EXTENSION_SOUNDFILEREADERMP3_HPP
#define SFML_EXTENSION_SOUNDFILEREADERMP3_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Audio/SoundFileReader.hpp>
#include <mpg123.h>

class SoundFileReaderMp3 : public sf::SoundFileReader {

public:

	////////////////////////////////////////////////////////////
	/// \brief Check if this reader can handle a file given by an input stream
	///
	/// \param stream Source stream to check
	///
	/// \return True if the file is supported by this reader
	///
	////////////////////////////////////////////////////////////
	static bool check(sf::InputStream& stream);


public:

	////////////////////////////////////////////////////////////
	/// \brief Copy constructor
	///
	////////////////////////////////////////////////////////////
	SoundFileReaderMp3(const SoundFileReaderMp3&) = default;

	////////////////////////////////////////////////////////////
	/// \brief Copy assignment operator
	///
	////////////////////////////////////////////////////////////
	SoundFileReaderMp3& operator=(const SoundFileReaderMp3&) = default;

	////////////////////////////////////////////////////////////
	/// \brief Default constructor
	///
	////////////////////////////////////////////////////////////
	SoundFileReaderMp3();

	////////////////////////////////////////////////////////////
	/// \brief Destructor
	///
	////////////////////////////////////////////////////////////
	~SoundFileReaderMp3() override;

	////////////////////////////////////////////////////////////
	/// \brief Open a sound file for reading
	///
	/// \param stream Source stream to read from
	/// \param info   Structure to fill with the properties of the loaded sound
	///
	/// \return True if the file was successfully opened
	///
	////////////////////////////////////////////////////////////
	bool open(sf::InputStream& stream, Info& info) override;

	////////////////////////////////////////////////////////////
	/// \brief Change the current read position to the given sample offset
	///
	/// The sample offset takes the channels into account.
	/// If you have a time offset instead, you can easily find
	/// the corresponding sample offset with the following formula:
	/// `timeInSeconds * sampleRate * channelCount`
	/// If the given offset exceeds to total number of samples,
	/// this function must jump to the end of the file.
	///
	/// \param sampleOffset Index of the sample to jump to, relative to the beginning
	///
	////////////////////////////////////////////////////////////
	void seek(sf::Uint64 sampleOffset) override;

	////////////////////////////////////////////////////////////
	/// \brief Read audio samples from the open file
	///
	/// \param samples  Pointer to the sample array to fill
	/// \param maxCount Maximum number of samples to read
	///
	/// \return Number of samples actually read (may be less than \a maxCount)
	///
	////////////////////////////////////////////////////////////
	sf::Uint64 read(sf::Int16* samples, sf::Uint64 maxCount) override;

private:

	////////////////////////////////////////////////////////////
	// Member data
	////////////////////////////////////////////////////////////
	mpg123_handle* m_handle;
	bool m_opened;
	int m_channelCount;
};


#endif //SFML_EXTENSION_SOUNDFILEREADERMP3_HPP
