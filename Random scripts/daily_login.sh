#!/bin/sh

divider() {
    printf "\n\033[1;36m%*s\033[0m\n" "$(tput cols)" '' | tr ' ' '='
}

cd "$HOME/Documents/42_Singapore_SUTD/Random scripts" || exit 1

divider
echo "Days that I logged in before 12nn"
divider

sh "./Login_Before_12.sh"

divider
echo "Hours logged for (50 hrs Intra_Award)"
divider

sh "./Hours_Logged_Info_(Intra_Award).sh"

divider
echo "sync proj. with github"
divider

zsh -ic 'syncproj'

divider
echo "cd to current project"
divider

zsh -ic 'curproj'
