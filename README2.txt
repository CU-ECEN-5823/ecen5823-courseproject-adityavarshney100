# Sluiter, Jan 2, 2021.
# Setup repository ecen5823-s21-assignments, a template repository.
#   Created default branch "main" to hold assignment 1 starter code.
#   Github Classroom required the starter code repo to be a "Template" repository,
#   we are taking a different route for ecen5823-s21-assignments and Github
#   Classroom this semester.
#
# Sluiter, Jan 5, 2021.
# Updated comment above regarding template repositories.
#   Deleted repository ecen5823-s21-assignments
#   Setup public repository ecen5823-s21-assignments, not a template repository.



# After the started code repo has been created in the cloud, from my starter code folder
# on my local machine:

# delete previous git data
rm -rf .git/ .github/

git init

# add all the files except files ignored by .gitignore 
git add .

git commit -m "Initial starter code"

# create a branch called master
git branch -M master

# set the origin
git remote add origin https://github.com/CU-ECEN-5823/ecen5823-s21-assignments.git

# push all of the files to the cloud
git push -u origin master

