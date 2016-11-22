#pragma once
#include <xstddef>
#include <string>


//-----------------------------------------------------------------------------------------------
typedef uint32 QuHash;


//-----------------------------------------------------------------------------------------------
class QuString
{
	static const int8 s_MIN_STRING_MEMORY_BUFFER = 32;

public:
	//-----------------------------------------------------------------------------------------------
	//CTORS AND DTORS
	QuString();
	QuString(const char* otherString);
	QuString(const QuString& otherString);
	QuString(QuString&& otherString);
	~QuString();
	//-----------------------------------------------------------------------------------------------

public:
	//-----------------------------------------------------------------------------------------------
	//GETTERS AND SETTERS
	size_t GetLength() const { return m_length; }
	size_t GetCapacity() const { return m_capacity; }
	const char* GetRaw() const { return m_stringPtr; }
	QuHash GetHash() const { return std::hash<std::string>{}(GetRaw()); }
	//-----------------------------------------------------------------------------------------------
	
public:
	//-----------------------------------------------------------------------------------------------
	//OPERATORS
	void operator=(const QuString& otherString);
	QuString operator+(const QuString& otherString) const;
	QuString operator+(const char* otherString) const;
	void operator+=(const QuString& otherString);
	//QuString& operator=(QuString&& otherString);
	bool operator==(const QuString& otherString) const;
	bool operator!=(const QuString& otherString) const;
	operator QuHash() const { return GetHash(); } //Easy hashing
	//-----------------------------------------------------------------------------------------------

public:
	//-----------------------------------------------------------------------------------------------
	//UTILITY FUNCTIONS
	static QuString F(const QuString& formatString, ...);

private:
	char* m_stringPtr;
	char m_internalBuffer[s_MIN_STRING_MEMORY_BUFFER];
	size_t m_capacity;
	size_t m_length;
};