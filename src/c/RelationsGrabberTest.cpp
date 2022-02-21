#include <iostream>

#include "RelationsGrabber.h"

std::vector<std::string> descriptions {
	R"foo(An epic to track progress on AWS for Solarwinds, and related activities

Parent 'Determine if CPC can leverage SolarWinds for AWS along with PagerDuty'
Parent 'Build crossaccount deployments of any SolarWinds infrastructure needed'
Parent 'Determine and implement SolarWinds monitoring for IT GitLab'
)foo",
	R"foo(While discussing the lack of alerts for TeamForge, we considered that perhaps
the virtual machine has been in an alerting state for as long as the
`/opt/collabnet` mount has been over it's threshold. In order to pursue this
theory, we would like to expand the disk space so that we aren't being
triggered.

Furthermore, we can use this task to create a new single disk with enough space
and `pvmove` LVM extents to the new disk in order to consolidate how many disks
we have attached to the machine.

Relates 'Confirm that alerts for Azure Linux machines are reaching PagerDuty and KONOS'
)foo"
};

int main(int argc, char *argv[])
{
	RelationSlugs relationSlugs {
		"Parent",
		"Start to start",
		"Start to finish",
		"Finish to start",
		"Finish to finish",
		"Relates",
		"Duplicates",
		"Blocks"
	};

	for ( auto& description : descriptions )
	{
		RelationsGrabber relationsGrabber {description, relationSlugs};

		auto [relations, newEnd, status] = relationsGrabber.grab();

		for ( auto& relation : relations )
		{
			std::cout << relation.RelationSlug << ": " << relation.Issue << "\n";
		}
	}
}
