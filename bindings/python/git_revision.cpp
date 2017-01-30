#include <boost/python.hpp>
#include <git_revision.h>
#include "git_revision.hpp"

void export_git_revision()
{
    boost::python::scope().attr("git_revision") = GIT_REVISION;
}
