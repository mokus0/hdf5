/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by the Board of Trustees of the University of Illinois.         *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the files COPYING and Copyright.html.  COPYING can be found at the root   *
 * of the source code distribution tree; Copyright.html can be found at the  *
 * root level of an installed copy of the electronic HDF5 document set and   *
 * is linked from the top-level documents page.  It can also be found at     *
 * http://hdf.ncsa.uiuc.edu/HDF5/doc/Copyright.html.  If you do not have     *
 * access to either file, you may request a copy from hdfhelp@ncsa.uiuc.edu. *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * Programmer:  Robb Matzke <matzke@llnl.gov>
 *              Friday, September 25, 1998
 *
 * Purpose:	Test H5Gunlink().
 */
#include "h5test.h"

const char *FILENAME[] = {
    "unlink",
    "lunlink",
    NULL
};

#define THE_OBJECT	"/foo"

/* Macros for test_create_unlink() */
#define GROUPNAME       "Group"
#define NGROUPS         1000


/*-------------------------------------------------------------------------
 * Function:	test_one
 *
 * Purpose:	Creates a group that has just one entry and then unlinks that
 *		entry.
 *
 * Return:	Success:	0
 *
 *		Failure:	number of errors
 *
 * Programmer:	Robb Matzke
 *              Friday, September 25, 1998
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static int
test_one(hid_t file)
{
    hid_t	work=-1, grp=-1;
    herr_t	status;
    
    /* Create a test group */
    if ((work=H5Gcreate(file, "/test_one", 0))<0) goto error;

    /* Delete by absolute name */
    TESTING("unlink by absolute name");
    if ((grp=H5Gcreate(work, "foo", 0))<0) goto error;
    if (H5Gclose(grp)<0) goto error;
    if (H5Gunlink(file, "/test_one/foo")<0) goto error;
    PASSED();

    /* Delete by local name */
    TESTING("unlink by local name");
    if ((grp=H5Gcreate(work, "foo", 0))<0) goto error;
    if (H5Gclose(grp)<0) goto error;
    if (H5Gunlink(work, "foo")<0) goto error;
    PASSED();

    /* Delete directly - should fail */
    TESTING("unlink without a name");
    if ((grp=H5Gcreate(work, "foo", 0))<0) goto error;
    H5E_BEGIN_TRY {
	status = H5Gunlink(grp, ".");
    } H5E_END_TRY;
    if (status>=0) {
	H5_FAILED();
	puts("    Unlinking object w/o a name should have failed.");
	goto error;
    }
    if (H5Gclose(grp)<0) goto error;
    PASSED();

    /* Cleanup */
    if (H5Gclose(work)<0) goto error;
    return 0;

 error:
    H5E_BEGIN_TRY {
	H5Gclose(work);
	H5Gclose(grp);
    } H5E_END_TRY;
    return 1;
}


/*-------------------------------------------------------------------------
 * Function:	test_many
 *
 * Purpose:	Tests many unlinks in a single directory.
 *
 * Return:	Success:	0
 *
 *		Failure:	number of errors
 *
 * Programmer:	Robb Matzke
 *              Friday, September 25, 1998
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static int
test_many(hid_t file)
{
    hid_t	work=-1, grp=-1;
    int		i;
    const int	how_many=500;
    char	name[32];
    
    /* Create a test group */
    if ((work=H5Gcreate(file, "/test_many", 0))<0) goto error;
    if ((grp = H5Gcreate(work, "/test_many_foo", 0))<0) goto error;
    if (H5Gclose(grp)<0) goto error;

    /* Create a bunch of names and unlink them in order */
    TESTING("forward unlink");
    for (i=0; i<how_many; i++) {
	sprintf(name, "obj_%05d", i);
	if (H5Glink(work, H5G_LINK_HARD, "/test_many_foo", name)<0) goto error;
    }
    for (i=0; i<how_many; i++) {
	sprintf(name, "obj_%05d", i);
	if (H5Gunlink(work, name)<0) goto error;
    }
    PASSED();

    /* Create a bunch of names and unlink them in reverse order */
    TESTING("backward unlink");
    for (i=0; i<how_many; i++) {
	sprintf(name, "obj_%05d", i);
	if (H5Glink(work, H5G_LINK_HARD, "/test_many_foo", name)<0) goto error;
    }
    for (i=how_many-1; i>=0; --i) {
	sprintf(name, "obj_%05d", i);
	if (H5Gunlink(work, name)<0) goto error;
    }
    PASSED();

    /* Create a bunch of names and unlink them from both directions */
    TESTING("inward unlink");
    for (i=0; i<how_many; i++) {
	sprintf(name, "obj_%05d", i);
	if (H5Glink(work, H5G_LINK_HARD, "/test_many_foo", name)<0) goto error;
    }
    for (i=0; i<how_many; i++) {
	if (i%2) {
	    sprintf(name, "obj_%05d", how_many-(1+i/2));
	} else {
	    sprintf(name, "obj_%05d", i/2);
	}
	if (H5Gunlink(work, name)<0) goto error;
    }
    PASSED();
    
    /* Create a bunch of names and unlink them from the midle */
    TESTING("outward unlink");
    for (i=0; i<how_many; i++) {
	sprintf(name, "obj_%05d", i);
	if (H5Glink(work, H5G_LINK_HARD, "/test_many_foo", name)<0) goto error;
    }
    for (i=how_many-1; i>=0; --i) {
	if (i%2) {
	    sprintf(name, "obj_%05d", how_many-(1+i/2));
	} else {
	    sprintf(name, "obj_%05d", i/2);
	}
	if (H5Gunlink(work, name)<0) goto error;
    }
    PASSED();
    

    /* Cleanup */
    if (H5Gclose(work)<0) goto error;
    return 0;
    
 error:
    H5E_BEGIN_TRY {
	H5Gclose(work);
	H5Gclose(grp);
    } H5E_END_TRY;
    return 1;
}


/*-------------------------------------------------------------------------
 * Function:	test_symlink
 *
 * Purpose:	Tests removal of symbolic links.
 *
 * Return:	Success:	0
 *
 *		Failure:	number of errors
 *
 * Programmer:	Robb Matzke
 *              Friday, September 25, 1998
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static int
test_symlink(hid_t file)
{
    hid_t	work=-1;

    TESTING("symlink removal");
    
    /* Create a test group and symlink */
    if ((work=H5Gcreate(file, "/test_symlink", 0))<0) goto error;
    if (H5Glink(work, H5G_LINK_SOFT, "link_value", "link")<0) goto error;
    if (H5Gunlink(work, "link")<0) goto error;

    /* Cleanup */
    if (H5Gclose(work)<0) goto error;
    PASSED();
    return 0;
    
 error:
    H5E_BEGIN_TRY {
	H5Gclose(work);
    } H5E_END_TRY;
    return 1;
}


/*-------------------------------------------------------------------------
 * Function:	test_rename
 *
 * Purpose:	Tests H5Gmove()
 *
 * Return:	Success:	0
 *
 *		Failure:	number of errors
 *
 * Programmer:	Robb Matzke
 *              Friday, September 25, 1998
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static int
test_rename(hid_t file)
{
    hid_t	work=-1, foo=-1, inner=-1;

    /* Create a test group and rename something */
    TESTING("object renaming");
    if ((work=H5Gcreate(file, "/test_rename", 0))<0) goto error;
    if ((foo=H5Gcreate(work, "foo", 0))<0) goto error;
    if (H5Gmove(work, "foo", "bar")<0) goto error;
    if ((inner=H5Gcreate(foo, "inner", 0))<0) goto error;
    if (H5Gclose(inner)<0) goto error;
    if (H5Gclose(foo)<0) goto error;
    if ((inner=H5Gopen(work, "bar/inner"))<0) goto error;
    if (H5Gclose(inner)<0) goto error;
    PASSED();

    /* Try renaming a symlink */
    TESTING("symlink renaming");
    if (H5Glink(work, H5G_LINK_SOFT, "link_value", "link_one")<0) goto error;
    if (H5Gmove(work, "link_one", "link_two")<0) goto error;
    PASSED();

    /* Cleanup */
    if (H5Gclose(work)<0) goto error;
    return 0;
    
 error:
    H5E_BEGIN_TRY {
	H5Gclose(work);
	H5Gclose(foo);
	H5Gclose(inner);
    } H5E_END_TRY;
    return 1;
}


/*-------------------------------------------------------------------------
 * Function:    test_create_unlink
 *
 * Purpose:     Creates and then unlinks a large number of objects
 *
 * Return:      Success:        0
 *              Failure:        number of errors
 *
 * Programmer:  Quincey Koziol
 *              Friday, April 11, 2003
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static int test_create_unlink(const char *msg, hid_t fapl)
{
    hid_t 	file, group;
    unsigned u;
    char 	groupname[1024];
    char	filename[1024];

    TESTING(msg);
   
    /* Create file */
    h5_fixname(FILENAME[1], fapl, filename, sizeof filename);
    if ((file=H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, fapl))<0)
    {
        H5_FAILED();
        puts("    Creating file failed");
        goto error;
    }

    /* Create a many groups to remove */
    for(u=0; u<NGROUPS; u++) {
        sprintf(groupname,"%s %u",GROUPNAME,u);
        if((group = H5Gcreate (file, groupname, 0))<0)
        {
            H5_FAILED();
            printf("group %s creation failed\n",groupname);
            goto error;
        }
        if(H5Gclose (group)<0)
        {
            H5_FAILED();
            printf("closing group %s failed\n",groupname);
            goto error;
        }
    } /* end for */

    /* Remove the all the groups */
    for(u=0; u<NGROUPS; u++) {
        sprintf(groupname,"%s %u",GROUPNAME,u);
        if(H5Gunlink (file, groupname)<0)
        {
            H5_FAILED();
            printf("Unlinking group %s failed\n",groupname);
            goto error;
        }
    } /* end for */

    /* Close file */
    if(H5Fclose(file)<0)
    {
        H5_FAILED();
        printf("Closing file failed\n");
        goto error;
    }

    PASSED();
    return 0;

error:
    return -1;
} /* end test_create_unlink() */


/*-------------------------------------------------------------------------
 * Function:	main
 *
 * Purpose:	Test H5Gunlink()
 *
 * Return:	Success:	zero
 *
 *		Failure:	non-zero
 *
 * Programmer:	Robb Matzke
 *              Friday, September 25, 1998
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
int
main(void)
{
    hid_t	fapl, fapl2, file;
    int		nerrors = 0;
    char	filename[1024];

    /* Metadata cache parameters */
    int mdc_nelmts;
    size_t rdcc_nelmts;
    size_t rdcc_nbytes;
    double rdcc_w0;

    /* Open */
    h5_reset();
    fapl = h5_fileaccess();
    h5_fixname(FILENAME[0], fapl, filename, sizeof filename);
    if ((file=H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, fapl))<0)
	goto error;

    /* Make copy of regular fapl, to turn down the elements in the metadata cache */
    if((fapl2=H5Pcopy(fapl))<0)
        goto error;

    /* Get FAPL cache settings */
    if(H5Pget_cache(fapl2,&mdc_nelmts,&rdcc_nelmts,&rdcc_nbytes,&rdcc_w0)<0)
        printf("H5Pget_cache failed\n");

    /* Change FAPL cache settings */
    mdc_nelmts=1;
    if(H5Pset_cache(fapl2,mdc_nelmts,rdcc_nelmts,rdcc_nbytes,rdcc_w0)<0)
        printf("H5Pset_cache failed\n");

    /* Tests */
    nerrors += test_one(file);
    nerrors += test_many(file);
    nerrors += test_symlink(file);
    nerrors += test_rename(file);
    
    /* Test creating & unlinking lots of objects with default FAPL */
    nerrors += test_create_unlink("create and unlink large number of objects",fapl);
    /* Test creating & unlinking lots of objects with a 1-element metadata cache FAPL */
    nerrors += test_create_unlink("create and unlink large number of objects with small cache",fapl2);
 
    /* Close */
    if (H5Fclose(file)<0) goto error;
    if (nerrors) {
	printf("***** %d FAILURE%s! *****\n", nerrors, 1==nerrors?"":"S");
	exit(1);
    }
    puts("All unlink tests passed.");
    h5_cleanup(FILENAME, fapl);
    return 0;
 error:
    return 1;
}

