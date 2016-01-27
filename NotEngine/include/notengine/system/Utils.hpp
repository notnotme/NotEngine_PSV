#ifndef UTILS_HPP
#define	UTILS_HPP

#pragma once
#include <string>

namespace NotEngine {

	namespace System {

		// Usefull things
		class Utils {

			private:
				Utils(const Utils& copy);
				void operator=(Utils const&);
				Utils();

			public:
				virtual ~Utils();

				/// Return the string content of a file
				static std::string stringFromFile(const std::string filename);
		};

	} // Namespace System

} // Namespace NotEngine

#endif /* UTILS_HPP */
