#ifndef SINGLETON_HPP
#define	SINGLETON_HPP

#pragma once

namespace NotEngine {

	namespace System {

		/**
		 * class Singleton
		 *
		 * A template to ease the writting of class used
		 * to implement the Singleton pattern.
		 */
		template<class T>
		class Singleton {
			private:
				/// Disallow copy
				Singleton(const Singleton& copy);
				void operator=(Singleton const&);

			protected:
				/// The internal unique instance holder
				static T* _instance;

				/// Disallow public instanciating
				Singleton () {};

			public:
				virtual ~Singleton () {};

				/// Return the unique instance of the object
				static T* instance () {
					if(_instance == 0) {
						_instance = new T();
					}

					return _instance;
				};

				/// delete the allocated instance
				static void deleteInstance() {
					if(_instance != 0)
						delete(_instance);
				}
		};

		template<class T>
		T* Singleton<T>::_instance = 0;

	} // namespace System

} // namespace NotEngine

#endif	/* SINGLETON_HPP */
