
find_library(libEnigma libEnigma)

add_executable(triangles 
	"src/main.cpp"
)
target_link_libraries(triangles PRIVATE libEnigma)
