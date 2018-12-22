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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SoundFileReaderMp3.hpp>
#include <SFML/System/InputStream.hpp>
#include <SFML/System/Err.hpp>
#include <iostream>

#define UNUSED_PARAMETER(x) (void)(x)

namespace
{
	ssize_t mpg123_handle_read(void* vstream, void* buf, size_t nbyte) noexcept
	{
		sf::InputStream* stream = reinterpret_cast<sf::InputStream*>(vstream);
		return stream->read(buf, static_cast<sf::Int64>(nbyte));
	}

	off_t mpg123_handle_lseek(void* vstream, off_t offset, int whence) noexcept
	{
		sf::InputStream* stream = reinterpret_cast<sf::InputStream*>(vstream);
		switch(whence)
		{
			case SEEK_SET:
				break;
			case SEEK_CUR:
				offset += static_cast<off_t>(stream->tell());
				break;
			case SEEK_END:
				offset += static_cast<off_t>(stream->getSize());
				break;
			default:
				return -1;
		}
		return static_cast<off_t>(stream->seek(offset));
	}

	void mpg123_handle_cleanup(void* vstream) noexcept
	{
		UNUSED_PARAMETER(vstream);
	}

	void set_mpg123_quiet(mpg123_handle* handle) noexcept
	{
		long mpg123_flags = 0;
		double dummy = 0;
		int error = mpg123_getparam(handle, MPG123_FLAGS, &mpg123_flags, &dummy);
		if(error != MPG123_OK)
		{
			sf::err() << "Failed to get mpg123 parameters: " << mpg123_plain_strerror(error)
			          << std::endl;
		}

		error = mpg123_param(handle, MPG123_FLAGS, mpg123_flags | MPG123_QUIET, dummy);
		if(error != MPG123_OK)
		{
			sf::err() << "Failed to set mpg123 parameters: " << mpg123_plain_strerror(error)
			          << std::endl;
		}
	}
} // namespace

////////////////////////////////////////////////////////////
bool SoundFileReaderMp3::check(sf::InputStream& stream)
{
	int error = mpg123_init();
	if(error != MPG123_OK)
	{
		sf::err() << "Failed init mpg123: " << mpg123_plain_strerror(error) << std::endl;
		return false;
	}

	mpg123_handle* handle = mpg123_new(nullptr, &error);
	if(error != MPG123_OK)
	{
		sf::err() << "Failed to open mpg123 for reading: " << mpg123_plain_strerror(error)
		          << std::endl;
		mpg123_exit();
		return false;
	}
	set_mpg123_quiet(handle);

	error = mpg123_replace_reader_handle(
	  handle, &mpg123_handle_read, &mpg123_handle_lseek, &mpg123_handle_cleanup);
	if(error != MPG123_OK)
	{
		sf::err() << "Failed to replace mpg123 reader handle: " << mpg123_plain_strerror(error)
		          << std::endl;
		mpg123_delete(handle);
		mpg123_exit();
		return false;
	}

	error = mpg123_open_handle(handle, &stream);
	if(error != MPG123_OK)
	{
		sf::err() << "Failed to open handle with mpg123: " << mpg123_plain_strerror(error)
		          << std::endl;
		mpg123_delete(handle);
		mpg123_exit();
		return false;
	}

	error = mpg123_scan(handle);
	if(error != MPG123_OK && error != MPG123_NEW_FORMAT)
	{
		sf::err() << "Failed to scan stream with mpg123: " << mpg123_plain_strerror(error)
		          << std::endl;
		mpg123_close(handle);
		mpg123_delete(handle);
		mpg123_exit();
		return false;
	}

	int errcode = mpg123_errcode(handle);
	if(errcode == MPG123_RESYNC_FAIL)
	{
		// Unsupported formats (like wav) fail here
		// It seems that MPEG is too magic to detect invalid format before
		mpg123_close(handle);
		mpg123_delete(handle);
		mpg123_exit();
		return false;
	}

	off_t length = mpg123_length(handle);
	if(length <= 0)
	{
		// Obviously unsupported formats (like jpg) fail here
		// It seems that MPEG is too magic to detect invalid format before
		mpg123_close(handle);
		mpg123_delete(handle);
		mpg123_exit();
		return false;
	}

	mpg123_close(handle);
	mpg123_delete(handle);
	mpg123_exit();
	return true;
}

////////////////////////////////////////////////////////////
SoundFileReaderMp3::SoundFileReaderMp3(): m_handle(nullptr), m_opened(false), m_channelCount(0)
{
	int error = mpg123_init();
	if(error != MPG123_OK)
	{
		sf::err() << "Failed init mpg123: " << mpg123_plain_strerror(error) << std::endl;
	}
}

////////////////////////////////////////////////////////////
SoundFileReaderMp3::~SoundFileReaderMp3()
{
	if(m_handle)
	{
		if(m_opened)
		{
			mpg123_close(m_handle);
		}
		mpg123_delete(m_handle);
	}
	mpg123_exit();
}

////////////////////////////////////////////////////////////
bool SoundFileReaderMp3::open(sf::InputStream& stream, sf::SoundFileReader::Info& info)
{
	int error = MPG123_OK;

	m_handle = mpg123_new(nullptr, &error);
	if(error != MPG123_OK)
	{
		sf::err() << "Failed to open mpg123 for reading: " << mpg123_plain_strerror(error)
		          << std::endl;
		return false;
	}
	set_mpg123_quiet(m_handle);

	error = mpg123_replace_reader_handle(
	  m_handle, &mpg123_handle_read, &mpg123_handle_lseek, &mpg123_handle_cleanup);
	if(error != MPG123_OK)
	{
		sf::err() << "Failed to replace mpg123 reader handle: " << mpg123_plain_strerror(error)
		          << std::endl;
		return false;
	}

	error = mpg123_open_handle(m_handle, &stream);
	if(error != MPG123_OK)
	{
		sf::err() << "Failed to open handle with mpg123: " << mpg123_plain_strerror(error)
		          << std::endl;
		return false;
	}
	m_opened = true;

	error = mpg123_scan(m_handle);
	if(error != MPG123_OK && error != MPG123_NEW_FORMAT)
	{
		sf::err() << "Failed to scan stream with mpg123: " << mpg123_plain_strerror(error)
		          << std::endl;
		return false;
	}

	const off_t length = mpg123_length(m_handle);
	if(length <= 0)
	{
		sf::err() << "Failed to get stream length with mpg123: " << mpg123_plain_strerror(error)
		          << std::endl;
		return false;
	}

	long rate;
	int channels;
	int encoding;
	error = mpg123_getformat(m_handle, &rate, &channels, &encoding);
	if(error != MPG123_OK && error != MPG123_NEW_FORMAT && rate > 0 && channels > 0)
	{
		sf::err() << "Failed to get stream format with mpg123: " << mpg123_plain_strerror(error)
		          << std::endl;
		return false;
	}
	info.sampleCount = static_cast<sf::Uint64>(length) * static_cast<sf::Uint64>(channels);
	info.channelCount = static_cast<unsigned int>(channels);
	info.sampleRate = static_cast<unsigned int>(rate);

	m_channelCount = channels;

	return true;
}

////////////////////////////////////////////////////////////
void SoundFileReaderMp3::seek(sf::Uint64 sampleOffset)
{
	const off_t new_offset =
	  mpg123_seek(m_handle, static_cast<off_t>(sampleOffset) / m_channelCount, SEEK_SET);
	if(new_offset < 0)
	{
		sf::err() << "Failed to seek with mpg123: "
		          << mpg123_plain_strerror(static_cast<int>(new_offset)) << std::endl;
	}
}

////////////////////////////////////////////////////////////
sf::Uint64 SoundFileReaderMp3::read(sf::Int16* samples, sf::Uint64 maxCount)
{
	size_t done = 0;
	const int error = mpg123_read(
	  m_handle, reinterpret_cast<unsigned char*>(samples), maxCount * sizeof(sf::Int16), &done);
	if(error != MPG123_OK && error != MPG123_NEW_FORMAT && error != MPG123_DONE)
	{
		sf::err() << "Failed to read stream with mpg123: " << mpg123_plain_strerror(error)
		          << std::endl;
	}
	return done / sizeof(sf::Int16);
}
