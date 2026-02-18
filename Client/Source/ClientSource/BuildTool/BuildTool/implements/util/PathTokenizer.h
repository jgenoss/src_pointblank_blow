#pragma once



bool GetPathToken(const Path& path, const size_t nth, std::string& outToken);

void GetPathTokenList(const Path& path, std::vector<std::string>& outTokenList);

void GetPathTokenList(const Path& path, std::vector<std::wstring>& outTokenList);

void GetPathTokenList(const Path& path, const size_t begin, const size_t end, std::vector<std::string>& outTokenList);

size_t GetNumberOfTokens(const Path& path);

void ClassifyCapitalAndSmallLetter(PathPtr& inout, const std::wstring& wstrWorkdir);